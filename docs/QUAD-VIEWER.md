# Using the Quad Viewer with OCOC

*Ships as `docs/QUAD-VIEWER.md` in the `calsynth/ococ` repo and as the text
under the release download.*

The Quad Viewer puts your module's screen — and a lot the screen can't show —
in a browser tab. It is a single web page at
**[ocusers.com/tools/quad-viewer](https://ocusers.com/tools/quad-viewer)**.
Nothing to install, no driver; the page talks to the USB port directly and
sends nothing anywhere else.

## You need

1. **OCOC in slot A or B.** The viewer needs the module to show up as a USB
   serial port, and only OCOC's A (MIDI + Audio) and B (MIDI) slots do that.
   Slots X and Y are stock Phazerville (X keeps USB audio) with no serial port,
   and will not connect — nor will a stock Phazerville download. Hold Z at
   power-up for the boot menu, then press A or B; keep Z held while pressing to
   save the choice.
2. **Chrome, Edge or Arc — or Firefox 151 or later.** WebSerial has been in the
   Chromium browsers for years; Firefox added it in 151 (May 2026). Firefox should
   work here but has not been tested — tell me either way. Safari has no WebSerial
   and there is no workaround.
3. **A USB data cable** to the module's front-panel USB-C (XLOC2 and other
   Expanded ORN8 modules) or the Teensy's micro-USB on the board. Not the USB
   host jack — that one is for MIDI controllers. If the module never shows up
   in the port list, the cable is charge-only; try another.

## Connecting

Plug in, open the page, click **Connect**, pick the port the browser offers
(named **OCOC**; `usbmodem…` on a Mac, `COM…` on Windows). The screen appears within a second.
The layout you set is remembered for next time.

## What the four panels can show

The page is a 2×2 grid. Each panel has its own dropdown:

- **Live screen** — a mirror of the OLED, any app, any page. What you see on
  the module, you see here.
- **Quadrants 4-up** — all four applets rendered at once, each full size,
  including the two the module's own screen is hiding. This is the view the
  firmware was built for.
- **Audio stack** with input and output level meters.
- **MIDI map**, or a live **MIDI monitor** of everything in and out, with
  pause, clear, direction and channel filters.
- **Scope A / Scope B** — any CV out, CV in, trigger in, audio in or out.
  Scale, time base and offset controls, plus freeze.
- **Off**.

The ⛶ button fills the window with one panel. **Classic** in the toolbar swaps
the whole grid for one big mirror of the OLED — the best choice for filming a
patch walk-through or screen-sharing.

## Driving the module from the computer

**Control** in the toolbar opens on-screen buttons and two endless encoders —
drag or scroll to turn them — so you can operate the module without reaching
for it. **MODE** switches the module between Quadrants and Audio Setup. The
preset dropdown loads a preset when you pick it, and **Save** writes the
current state into the selected slot.

Keyboard, once connected:

| Key | Does |
|---|---|
| `f` | fullscreen on/off (recording mode) |
| `Space` | grid ⇄ Classic |
| `Esc` | leave fullscreen / close help |
| **Quadrants mode** | |
| `↑ ↓ ← →` or `W A S D` | focus a quadrant |
| `X` | edit — select parameter |
| `Z` / `C` | value + / − |
| `Q` / `E` | previous / next applet |
| `O` / `P` | previous / next preset |
| **Audio mode** (click MODE) | |
| `↑ ↓` or `W S` | cursor up / down |
| `Z` / `C`, `A` / `D` | value + / − |
| `Enter` or `X` | select slot / confirm applet |
| `Backspace` | back to cursor mode |
| hold `Shift` | the same keys drive the right side |

**Color** cycles the phosphor tint; **Save PNG** captures the current view.
The **Help** button on the page repeats all of this.

## If nothing shows up

- No port in the list → charge-only cable, or the module is in slot X or Y
  (stock Phazerville). Hold **Z** at power-up for the boot menu, then press
  **A** or **B** (keep Z held to save).
- Port is there but the screen stays blank → click Disconnect, power-cycle the
  module, Connect again. Another program (Arduino IDE serial monitor, a DAW's
  MIDI-over-serial bridge) holding the port will also do this — close it.
- Browser says WebSerial isn't available → you're in Firefox or Safari.
- 4-up view is empty but the mirror works → the module is not in Quadrants;
  click **MODE**.
- USB audio garbles in either direction (module→computer with the module's
  own outputs clean, or computer→module heard on the analog outs) → first set
  the DAW's I/O buffer to 128 samples or more (the firmware's limit; 64
  provokes it, in stock Phazerville too); then deselect and reselect OCOC as the audio device in
  your DAW, or unplug and reconnect the USB cable; the stream restarts clean.
- The module rebooted on its own → it keeps a report of the last crash. With
  the viewer disconnected, send the four characters `O?!!` to the port **in
  one go** (paste them, or use the command below — typed one key at a time
  they arrive too slowly and nothing happens). Send us what it prints before
  `ENDCRASH`; `NOCRASH` means nothing was recorded. On a Mac, in Terminal:

  ```
  PORT=$(ls /dev/cu.usbmodem* | head -1); stty -f $PORT raw -echo
  cat $PORT & sleep 1; printf 'O?!!' > $PORT; sleep 2; kill %1
  ```

## Building your own

The wire protocol is documented in `PROTOCOL.md` in this repo if you want to
build your own viewer.
