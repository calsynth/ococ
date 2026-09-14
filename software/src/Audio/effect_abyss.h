#pragma once
// Teensy Audio Library wrapper for AbyssCore (see blackhole_core.h).
// Stereo in, stereo out, wet-only (the applet handles dry/wet mixing).
//
// Delay memory (~150 KB at 48 kHz) is allocated in one arena on the RAM2
// heap. There is deliberately NO PSRAM fallback (OCOC v0.1, 2026-09-14): a
// PSRAM-resident instance costs >50 % CPU for the tank's scattered reads, and
// on the bench a third instance loaded that way took the module down with a
// hard fault in the USB audio receive path. Out of RAM2 = the applet refuses
// to start and says so.

#include <Arduino.h>
#include <AudioStream.h>
#include "abyss_core.h"

class AudioEffectAbyssReverb : public AudioStream {
public:
  AudioEffectAbyssReverb() : AudioStream(2, inputQueueArray) {}
  ~AudioEffectAbyssReverb() { end(); }

  // Allocate buffers and start processing. Returns false if out of memory.
  // RAM2 heap only: the tank does ~30 scattered reads per sample, and PSRAM
  // cache misses on those made v1 cost >50% CPU (see the header comment).
  bool begin() {
    if (arena) return core.Ready();
    const size_t bytes = core.RequiredBytes(AUDIO_SAMPLE_RATE_EXACT);
    arena = malloc(bytes);
    if (!arena) return false;
    const bool ok = core.Init(AUDIO_SAMPLE_RATE_EXACT, arena, bytes);
    if (!ok) {
      FreeArena();
      return false;
    }
    return true;
  }

  void end() {
    __disable_irq();
    core.Release();
    __enable_irq();
    FreeArena();
  }

  bool ready() const { return core.Ready(); }

  // parameter passthroughs (single-word stores; ISR-safe)
  void setGravity(float g) { core.SetGravity(g); }        // -1..1
  void setSize(float s) { core.SetSize(s); }              // 0..1
  void setPredelayMs(float ms) { core.SetPredelay(ms * 0.001f); }
  void setModDepth(float d) { core.SetModDepth(d); }      // 0..1
  void setModRate(float hz) { core.SetModRate(hz); }
  void setLoCut(float v) { core.SetLoCut(v); }            // 0..1
  void setHiDamp(float v) { core.SetHiDamp(v); }          // 0..1
  void setMonoSum(bool b) { core.SetMonoSum(b); }
  void setInputGain(float g) { in_gain = g; }
  void freeze() {} // reserved

  virtual void update(void) override {
    audio_block_t* inL = receiveReadOnly(0);
    audio_block_t* inR = receiveReadOnly(1);
    if (!core.Ready()) {
      if (inL) release(inL);
      if (inR) release(inR);
      return;
    }
    audio_block_t* outL = allocate();
    audio_block_t* outR = allocate();
    if (!outL || !outR) {
      if (outL) release(outL);
      if (outR) release(outR);
      if (inL) release(inL);
      if (inR) release(inR);
      return;
    }

    const int16_t* pl = inL ? inL->data : nullptr;
    // mono sources patched to ch0 only: mirror into the right input
    const int16_t* pr = inR ? inR->data : pl;

    for (int n = 0; n < AUDIO_BLOCK_SAMPLES; ++n) {
      const float l = pl ? pl[n] * (in_gain / 32768.0f) : 0.0f;
      const float r = pr ? pr[n] * (in_gain / 32768.0f) : 0.0f;
      float wl, wr;
      core.Process(l, r, wl, wr);
      int32_t sl = static_cast<int32_t>(wl * 32767.0f);
      int32_t sr_ = static_cast<int32_t>(wr * 32767.0f);
      if (sl > 32767) sl = 32767; else if (sl < -32768) sl = -32768;
      if (sr_ > 32767) sr_ = 32767; else if (sr_ < -32768) sr_ = -32768;
      outL->data[n] = static_cast<int16_t>(sl);
      outR->data[n] = static_cast<int16_t>(sr_);
    }

    transmit(outL, 0);
    transmit(outR, 1);
    release(outL);
    release(outR);
    if (inL) release(inL);
    if (inR) release(inR);
  }

private:
  void FreeArena() {
    if (!arena) return;
    free(arena);
    arena = nullptr;
  }

  audio_block_t* inputQueueArray[2];
  AbyssCore core;
  void* arena = nullptr;
  float in_gain = 1.0f;
};
