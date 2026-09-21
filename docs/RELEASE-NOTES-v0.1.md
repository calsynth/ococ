**Based on** Phazerville Suite v2.0.1 · **Hardware:** Teensy 4.1 O&C (XLOC2 and other T4.1 builds) · **Hex:** `ococ-0.1_T41-129ff872.hex`

OCOC is a development of Phazerville that adds some experimental features that may or
may not get picked up by the main firmware down the line.

The biggest thing in this first release is the **Quad Viewer**, a web-based app that
greatly expands the display of information from your XLOC2 or other Teensy 4.1 ORN8
(Ornament & Crime) module.

## Quad Viewer

There is not much to do other than hook up a cable and go to a web page.

- Flash OCOC and boot into **slot A** (OCOC, MIDI + Audio) or **slot B** (OCOC, MIDI).
  Those are the two slots that show up as a serial port. Slots X and Y are stock
  Phazerville and will not connect.
- You will need a **USB data cable** (not a charge-only one) between the module and your
  computer — the front-panel USB-C on XLOC2 and other Expanded ORN8 modules, or the
  Teensy's micro-USB on the board. Not the USB host jack; that one is for MIDI
  controllers.
- It works in Chrome, Edge or Arc. Firefox added Web Serial in version 151, so that
  should work too. No Safari, and nothing else that does not support Web Serial.

Head to the viewer page at **https://ocusers.com/tools/quad-viewer/app/**, click **Connect**,
and pick the module's serial port from the list the browser shows you — it is named
**OCOC** (`usbmodem…` on a Mac, `COM…` on Windows). It should immediately come to life
and show you what is going on in your module.

### The panes

The window is a 2×2 grid and each pane can be set as you like: all four Quadrants
applets at once, a live mirror of the module screen, the audio stack with in/out meters,
your MIDI maps, a live MIDI monitor, either of the two scopes, or off. The ⛶ button in a
pane fills the window with that pane. The layout is remembered for next time you open the viewer.

Hit the **Help** button any time for a popup of the general instructions and keyboard
shortcuts.

You can also control the module from the browser with a mouse — hit **Control** at the
top and the on-screen arrows, encoders and keys that appear send the same encoder turns
and button presses as the front panel.

At the bottom of a pane that is showing Quadrants you will see controls for selecting
apps and presets and entering values on a selected app. When you change something in the
browser, you will see it change on your module.

**Audio + levels** shows the current audio stack, and when you have audio plugged into
the audio ins, L and R input and output levels on a bar meter.

A **scope** pane shows whatever is selected in the SRC dropdown at the bottom of the
scope — CV ins and outs, audio ins and outs, or triggers. Also at the bottom of the pane
are controls for scale, time, offset, reset and Freeze. This is a great tool for monitoring anything in your rig.

A **MIDI monitor** pane shows what is going in and out, with pause, clear and direction
filters.

Back to the top. The **preset** dropdown selects a preset, or select a blank slot to save
the changes you have made with the **Save** button next to it. **Classic** shows the
classic 2-up viewer, essentially a mirror of the screen on the module. **Color** changes
the colour of the information in the panes. **Save PNG** saves a screenshot of the
current view. **Fullscreen** drops the toolbars and fills the screen with just the panes
— useful if you need to do a screen recording.

## Abyss

The other addition over stock Phazerville is a nice reverb called **Abyss** in the audio
stack. It makes a nice end-of-chain effect and, as with all the audio goodies, can be
used independently of the CV ins and outs.

Everything else in the firmware is Phazerville, apart from the plumbing the viewer needs
and a fix to the USB audio path.

## Going back to stock

If you ever do want to switch to stock Phazerville, hold the **Z** button at start-up and
it will give you that choice from a menu — **A** = OCOC MIDI + Audio, **B** = OCOC MIDI,
**X** = Phazerville MIDI + Audio, **Y** = Phazerville MIDI. Press the one you want; keep
Z held while you press it to make it the default.

## Questions, bugs, ideas

This is all at the experimental stage, so if you have any questions, find any bugs, or
have any suggestions for how this could be improved, please open an issue on the repo — click **New issue** and pick Bug report, Question / support or
Feature idea. It is the only way I can keep track of everything, and it means the answer
is visible to the next person who hits the same thing. Have a look through the open and
closed issues first, and at the Known limits below — a few of the common
ones (USB audio wants a DAW buffer of 128 samples or more, one Abyss at a time, which
browsers do Web Serial) are already answered there.

## Known limits

- The viewer needs WebSerial: Chrome, Edge or Arc, or Firefox 151 or later
  (Firefox added WebSerial in May 2026; it has not been tested here). Safari has none.
- Slots X and Y behave as stock Phazerville, but they are built here rather than being
  upstream's own download, so they are not bit-identical to it.
- Calibration is shared across all four slots (it lives in the emulated
  EEPROM, as upstream).
- **Abyss: one instance at a time, mono or stereo.** A second one will not start — it
  says "Out of RAM!!". Once Abyss has run it holds its memory for the rest of the
  session, so you can unload and reload it freely as you change presets and applets.
- OCOC enumerates as a 4-in/4-out USB audio interface (as upstream). macOS
  screen recording treats it like a microphone: one channel, mono, with the
  mic processing path. To capture the module in stereo, record through a DAW
  or through an Aggregate/virtual device (Audio MIDI Setup, Loopback,
  BlackHole) that pairs channels 1–2.
- Presets store the audio applet stack only when you save the preset (as
  upstream): switching presets discards unsaved changes, Abyss included.
- After a preset load, an audio slot's *inactive* mode keeps the previous
  preset's applets (as upstream — confirmed in stock): toggling a blank slot
  with both encoders brings back whatever the last preset had there. Select
  the applet you want after toggling.
- **USB audio needs a DAW I/O buffer of 128 samples or more.** The audio engine works
  in 128-sample blocks, so with the DAW set to 64 the stream from the computer into
  the module garbles (you hear it on the module's analog outs; the module's own audio
  stays clean). Stock Phazerville behaves the same. **Set the DAW I/O buffer to 128 or
  larger** — 256 or 512 if you can spare the latency. If a stream does garble it stays
  garbled until it is restarted: deselect and reselect OCOC as the audio device in your
  DAW, or unplug and reconnect the USB cable. No reboot needed.

## Under the hood

The full technical notes are in the repo: **[docs/TECHNICAL-NOTES-v0.1.md](https://github.com/calsynth/ococ/blob/ococ/docs/TECHNICAL-NOTES-v0.1.md)**
— what this build is based on, everything that changed and why, the RAM1 diet, the USB
audio ordering fix, what was withdrawn and what is reported upstream. If you work on
this firmware rather than just run it, start there.

## Credits and licence

See the README. MIT inherited from upstream with the GPLv3 caveat; every file
header kept. Phazerville Suite (djphazer), Hemisphere Suite (Jason Justian),
Ornament & Crime (Dowling / Stadler / Churches), Mutable Instruments (Émilie Gillet).
