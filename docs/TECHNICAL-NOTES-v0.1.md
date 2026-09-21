# OCOC v0.1 — technical notes

*The detail behind [the release](RELEASE-NOTES-v0.1.md). Known limits, the boot menu and
the Quad Viewer walk-through are on the release page and are not repeated here.*

**Based on:** Phazerville Suite v2.0.1 · **Hex:** `ococ-0.1_T41-129ff872.hex` (2026-09-15; earlier candidates `dc4e8dea`, `dcb4af5d`, `8b991f53`, `658bfd2f`, `1a3959e7`, `9369c638`, `c9477481`, `1877b8b9`, `80033d30` superseded) · **Hardware:** Teensy 4.1 O&C (XLOC2 and other T4.1 builds)

First public release. It is the bench-verified quad-viewer build of 2026-08-26
(`92ff4803`) plus two applet registrations, a compiler pin, a RAM1 diet
(`80033d30`: cold UI code moved to flash so the two applets fit in ITCM), the
removal of the unused USB-MIDI SysEx mirror, and the OCOC boot screen.

## What's in it

- **Display suite** (PROTOCOL v1.4): quad capture (all four Quadrants applets
  rendered at once), live screen mirror, remote control from the computer, two
  oscilloscopes (any CV/trigger/audio in or out, scale, time base, offset,
  freeze), MIDI in/out monitor, audio in/out level meters. Works with the
  [Quad Viewer](https://ocusers.com/tools/quad-viewer/app/) in a Chromium browser —
  nothing to install.
- **Abyss** — reverb, mono and stereo, in the audio applet list in the A and B
  slots. One instance per module: it takes ~230 KB of RAM2, a second shows
  "Out of RAM!!" and does not start (the PSRAM fallback that used to let extra
  instances load was removed after it crashed the module on the bench — see Known limits on
  the release page). (Animorf, the MuRF-style filter bank, was withdrawn on the 2026-09-14
  bench — its output degrades into ring-mod-like distortion within about 90 s —
  and returns in a later release once the DSP is fixed.)
- **USB serial** in the A and B slots (`USB_MIDI_AUDIO_SERIAL` and
  `USB_MIDI_SERIAL`), so the module shows up as a serial port for the viewer.
- **Slots X and Y** are stock Phazerville v2.0.1 — MIDI + Audio and MIDI —
  built from the same tree without the OCOC additions, with upstream's USB
  types and upstream's own boot screen. The MTP/O&C-apps slot is gone.
- Compiler pinned to gcc 15.2.1 (`toolchain-gccarmnoneeabi-teensy 1.150201.0`),
  the version behind the benched binary.
- **RAM1 diet** (`80033d30`): 19 cold, non-template UI functions (Quadrants
  preset save/load and view, Settings, Clock Setup, EnvSeq UI handlers, MIDI
  In/Out monitors, FMDrum/HarmOsc/WTVCO start and view, the popup) moved to
  flash as strong FLASHMEM definitions. Pure moves; nothing on the audio or
  16.7 kHz paths. Needed because under LTO GCC ignores FLASHMEM on in-class
  and template functions, so the applets could not be trimmed directly.
- **USB audio and heavy applets**: the USB in/out objects now update first in
  every audio pass, so the USB output's clock recovery no longer inherits the
  timing swing of whatever runs before it (Abyss made the module→computer
  stream click while the analog outs stayed clean — an upstream driver
  behaviour, see `docs/usb-audio-glitch-diagnosis.md`; bench-verified clean with
  Abyss for 5 min). Costs one 128-sample block (2.7 ms) of latency on the USB output
  only. OCOC slots only. It does not change the small-buffer limit in the
  other direction — see Known limits on the release page.
- **USB-MIDI SysEx mirror removed** (2026-09-14 audit): the iPad/Android
  transport had no client and was never exercised; dropping it returned 4 KB
  of DTCM to the stack margin (6,149 B of objects; the rest went to `.bss` packing). The viewer is WebSerial only, as before.
- **Boot screen says OCOC**: title `OCOC v0.1` + build id + a plain load bar
  (no icon row, no zaps); the welcome card reads, on five centred lines,
  "WELCOME TO OCOC / A SLIGHTLY MODIFIED / EXPERIMENTAL BUILD / THAT IS
  MOSTLY / PHAZERVILLE". Setup/About in OCOC reads OCOC v0.1 / Calsynth / build id /
  github.com/calsynth. The module enumerates on USB as **OCOC** in A and B.
  The Phazerville lightning-bolt glyph is a sun in OCOC everywhere it appears
  (menu cursor, preset marker, applet icons, screensaver); the About page has a
  fixed sun each side of the name instead of the animated icon roulette.
  Slots X/Y keep the Phazerville splash, About page, glyphs, version and USB name.

## Boot menu

Hold Z at power-up for the menu, then a button; hold Z to save the choice.
**A** = OCOC MIDI + Audio · **B** = OCOC MIDI · **X** = Phazerville MIDI + Audio · **Y** = Phazerville MIDI.

## See also

- **[RELEASE-NOTES-v0.1.md](RELEASE-NOTES-v0.1.md)** — the release page, including the
  **Known limits** referred to above.
- **[usb-audio-glitch-diagnosis.md](usb-audio-glitch-diagnosis.md)** — the full account of
  the USB audio clock-recovery problem and the fix in this build.
- **[QUAD-VIEWER.md](QUAD-VIEWER.md)** — using the viewer.
