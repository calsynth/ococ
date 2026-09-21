# USB-audio artifacts with heavy applet chains — diagnosis

**Build:** T41_audio (v2.0.1, 4-ch USB audio).
**Repro:** audio chain `Input → Delay (6 taps) → Abyss → 3-Bandz`, streaming to the computer over USB audio. Occasional dropouts/clicks appear in the USB stream. Removing *either* Abyss or 3-Bandz makes them stop. CPU shown in the audio app stats stays under 60%.
**Key observation:** the unit's own audio outs are clean while the USB stream glitches.

## Why it isn't the DSP chain

Clean analog outs mean the audio graph is producing correct blocks every cycle — both outputs are fed from the same `output_route`. The only part of the signal path that behaves differently per output is `AudioOutputUSB`, i.e. the one component that has to bridge two clock domains: the audio-update ISR (crystal-timed off I2S DMA) and the host's USB frame clock. That's where these artifacts are being made, and the code (`teensy_cores/cores/teensy4/usb_audio_interface.{h,cpp}` @ 2adfda3c, via the framework-arduinoteensy pin) shows two concrete mechanisms, both of which scale with chain load while being invisible to the CPU% display.

## Mechanism A — the TX rate-matching controller loses lock under timing jitter

Device→host audio goes through a small ring (`ringTxBufferSize` = 5 blocks/channel; `TARGET_TX_BUFFER_TIME_S` = 3.5 ms, `usb_audio_interface.h:144,169`). The transmit callback runs on the USB ISR and drains 44/45 samples per frame; `AudioOutputUSB::update()` refills one block per audio cycle. Because the two clocks drift, the callback estimates ring occupancy and nudges the per-frame sample count by ±1 (`ASYNC_TX_ENDPOINT` path).

The occupancy estimate is only partly a block count — the fractional part (`virtualSamples`, `usb_audio_interface.cpp:836–851`) is derived from *when* `update()` was last called, using smoothed `ARM_DWT_CYCCNT` timestamps. That call time is not hardware-timed: it slides with everything that executes earlier in the same audio update pass. This chain adds a lot of *variance*, not just mean load:

- **Abyss**: ~30 scattered reads per sample across a ~150 KB delay arena — cost swings with data-cache state.
- **3-Bandz**: 4 state-variable filters + 6 `AudioEffectDynamics` instances (per-sample log/exp approximations), ×2 channels.
- **Delay, 6 taps**: 12 interpolated tap reads per sample out of a PSRAM ring — every miss is a bus stall.

The correction logic has a ±2-sample hysteresis and requires **10 consecutive same-sign deviations** before acting; a sign flip resets the counter (`updateDevCounter`/`updateTarget`, `usb_audio_interface.cpp:550–585`). Once the occupancy estimate is noisier than the hysteresis window, the counter keeps resetting, the ±1-sample corrections stop being applied, and the steady device/host clock drift accumulates uncorrected until the ring genuinely under- or overruns. Recovery is `resetTransmissionIndex()` plus zero-fill (`usb_audio_interface.cpp:868–881`) — an audible click or gap, **on USB only**. `txUsb_audio_underrun_count` / `txUsb_audio_overrun_count` count exactly these events.

This explains the "either applet" behaviour: it's not one bad applet, it's total per-pass timing variance crossing the threshold where the controller can no longer see the drift through the noise.

## Mechanism B — audio block pool exhaustion that only the USB side feels

`AudioInputUSB` allocates audio blocks **inside the USB ISR** (`usb_audio_receive_callback → tryIncreaseIdxIncoming → allocateChannels`, `usb_audio_interface.cpp:266–277`), and the USB ISR preempts the audio update mid-pass — precisely the moment transient pool usage peaks with a long chain (each SVF can hold 3 blocks in flight, every mixer/effect allocates its output before releasing inputs). `AudioOutputUSB::update()` also allocates a silence block whenever its input queue is momentarily empty (`usb_audio.cpp:174–186`). If the 252-block pool (`src/AudioIO.h`) touches empty at that instant, the USB streams degrade silently (`rxMemoryUnderrunCounter`) while the analog path never notices. Average CPU% says nothing about this either.

## Confirming which one it is

The interface already counts every one of these events — `USBAudioOutInterface::getStatus()` / `USBAudioInInterface::getStatus()` — but nothing in the firmware currently reads them. Surfacing them makes the diagnosis one keypress while the artifact is happening. The following compiles clean on v2.0.1 `T41_audio`:

In `src/AudioIO.h`, add to the namespace:

```cpp
void PrintUsbAudioStatus();
```

In `src/AudioIO.cpp`, add inside `namespace OC { namespace AudioIO {`:

```cpp
void PrintUsbAudioStatus() {
#ifdef AUDIO_INTERFACE
  const auto tx = output_usb.getStatus();
  const auto rx = input_usb.getStatus();
  Serial.printf("=== USB audio TX (unit -> computer) ===\n");
  Serial.printf("  underruns:%lu overruns:%lu  (each one = an audible reset/click)\n",
                tx.usb_audio_underrun_count, tx.usb_audio_overrun_count);
  Serial.printf("  drift corr: -1smp:%lu +1smp:%lu skip:%lu pad:%lu\n",
                tx.num_send_one_less, tx.num_send_one_more,
                tx.num_skipped_Samples, tx.num_padded_Samples);
  Serial.printf("  buffered:%.1f smp (target %.1f) ring:%u blk/ch ch:%u hs:%u streaming:%u\n",
                output_usb.getBufferedSamplesSmooth(),
                tx.target_num_buffered_samples, tx.ring_buffer_size,
                tx.num_transmitted_channels, tx.usb_high_speed, tx.transmittingData);
  Serial.printf("=== USB audio RX (computer -> unit) ===\n");
  Serial.printf("  underruns:%lu overruns:%lu memUnderruns:%lu\n",
                rx.usb_audio_underrun_count, rx.usb_audio_overrun_count,
                rx.audio_memory_underrun_count);
  Serial.printf("  buffered:%.1f smp (target %.1f) receiving:%u\n",
                input_usb.getBufferedSamplesSmooth(),
                rx.target_num_buffered_samples, rx.receivingData);
  Serial.printf("=== audio lib ===\n");
  Serial.printf("  blocks now:%u max:%u of %d   cpu now:%.1f%% max:%.1f%%\n",
                AudioMemoryUsage(), AudioMemoryUsageMax(), AUDIO_MEMORY,
                AudioProcessorUsage(), AudioProcessorUsageMax());
  Serial.printf("  (note: max counters are also reset ~1x/sec by the audio app's stats display)\n");
#else
  Serial.println("No USB audio in this build.");
#endif
}
```

In `src/Main.cpp`, in the serial command switch (before the `default:` capture-request case):

```cpp
#ifdef ARDUINO_TEENSY41
          case 'u':
            OC::AudioIO::PrintUsbAudioStatus();
            break;
#endif
```

Then, with the chain running and the artifact occurring, press `u` in a serial terminal a couple of times:

- **TX `underruns`/`overruns` incrementing in step with audible artifacts** → mechanism A (controller losing lock, ring resets).
- **RX `memUnderruns` climbing, or blocks-max pinned near 252** → mechanism B (pool exhaustion in the USB ISR).
- **Only `-1smp`/`+1smp` ticking slowly** → that's healthy drift correction; look elsewhere.

## Candidate fixes, depending on which counter moves

- **A:** deepen the TX ring — `TARGET_TX_BUFFER_TIME_S` 3.5 ms → ~8 ms buys the controller room at the cost of a few ms of USB latency. (The `#define` isn't `#ifndef`-guarded, so today it can't be overridden from a build flag — guarding it would make experimenting easy.) Alternatively, widen/filter the occupancy estimate so heavy update passes don't reset the hysteresis counter.
- **B:** raise `AUDIO_MEMORY`, and/or pre-allocate the RX ring so no `allocate()` happens in ISR context.
