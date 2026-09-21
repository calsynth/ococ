# OCOC

**OCOC** is Calsynth's open firmware for the XLOC2 and other Teensy 4.1
Ornament & Crime hardware. The name is Orange County + O&C hardware; it is a
Calsynth project and is not affiliated with or endorsed by the Ornament & Crime
or Phazerville projects.

It is **based on Phazerville Suite v2.0.1** with three additions:

- **The display suite** — the module's screen, all four Quadrants applets,
  audio meters, two oscilloscopes and a MIDI monitor, live in a browser over
  USB, with remote control of the module from the computer. This is what the
  [Quad Viewer](https://ocusers.com/tools/quad-viewer) talks to.
- **Abyss** (reverb) turned on in the audio applet list. It was already in the
  Phazerville source, credited to Calsynth; here it is registered. One
  instance per module; a second says "Out of RAM!!" and stays off. (Animorf,
  the companion filter bank, is held back until its DSP is fixed.)
- A USB serial port in the MIDI and MIDI + Audio slots, which is what makes the
  viewer possible.

No app or applet has been removed. Stock Phazerville v2.0.1 rides along in two
boot slots of its own, so you can switch back at power-up.

## Download and flash

Grab `ococ-<version>_T41-<commit>.hex` from the latest
[release](../../releases). Flash it with the
[Teensy Loader](https://www.pjrc.com/teensy/loader.html) exactly as you would an
official Phazerville hex: open the file, press the button on the Teensy (or
reboot from the module's menu), done. One hex carries all four slots.

**Everything you need to know about the boot menu:** hold **Z** while powering
up and the menu appears; press **A**, **B**, **X** or **Y** to boot that slot
this once. To make it stick, keep **Z** held while you press the slot button.

| Button | Slot | USB appears as |
|---|---|---|
| **A** | OCOC, MIDI + Audio | serial port + MIDI + 4-channel USB audio |
| **B** | OCOC, MIDI | serial port + MIDI |
| **X** | Stock Phazerville v2.0.1, MIDI + Audio | MIDI + 4-channel USB audio (as upstream) |
| **Y** | Stock Phazerville v2.0.1, MIDI | MIDI (as upstream) |

The Quad Viewer works in **A** and **B**; X and Y are upstream Phazerville as
released, with its own boot screen. If you never touch the menu the module boots
into whichever slot you last saved, as before.

## USB audio: set your DAW buffer to 128 or more

128 samples is the firmware's limit — the audio engine runs in 128-sample
blocks and the USB audio driver (inherited from upstream; stock Phazerville is
the same) needs the DAW's I/O buffer at least that large. At 64 the
computer→module stream garbles. If a stream does garble, deselect and reselect OCOC as the
audio device in the DAW, or unplug and reconnect the USB cable — it restarts
clean, no reboot. Details in the release notes.

## Using the Quad Viewer

See [`docs/QUAD-VIEWER.md`](docs/QUAD-VIEWER.md) — what it shows, how to connect,
the keyboard shortcuts, and what to do when nothing shows up.

## What actually changed

[`docs/TECHNICAL-NOTES-v0.1.md`](docs/TECHNICAL-NOTES-v0.1.md) is the build-level
account: what this is based on, every change and why, the RAM1 diet, the USB audio
ordering fix, what was withdrawn and what is reported upstream.
[`docs/usb-audio-glitch-diagnosis.md`](docs/usb-audio-glitch-diagnosis.md) is the full
diagnosis behind that fix.

## Building

Standard PlatformIO. Four environments make the release hex, in this order:

```
cd software
pio run -e T41 -e T41_audio -e T41_stock_audio -e T41_stock
```

`T41_stock` is last because its post-build step assembles the four slot images
into one multiboot hex. The two `stock` environments are the same tree built
without `-DOCOC` and `-DQUAD_CAPTURE`, with upstream's USB types. The compiler is pinned in `platformio.ini` to the version
the release was benched with. `teensy_size` is the gate that matters: if it
reports a negative "free for local variables" figure the module will come up
with a blank screen, so treat that as a failed build.

## Relationship to upstream

OCOC tracks [Phazerville Suite](https://github.com/djphazer/O_C-Phazerville)
(`upstream` remote). It exists so that Calsynth's display tools ship on
Calsynth's schedule; when the display suite lands upstream this fork will rebase
onto it and shrink to the applet enables and USB configuration. Upstream fixes
are merged in as they appear. The two `T41_stock*` environments — built without
`-DOCOC`, `-DQUAD_CAPTURE` and `-DUSB_MIDI_SERIAL` — are what runs in slots X and
Y: behaviourally stock Phazerville v2.0.1, though not bit-identical to the
official hex.

## Questions, bugs, feature ideas

Please use [Issues](../../issues) on this repo rather than emailing — it's
the only way I can track requests and keep the answer visible to the next
person who hits the same thing. Click **New issue** and pick a template:
**Bug report** for something broken, **Question / support** for "how do
I...", **Feature idea** for something OCOC doesn't do yet. Check open and
closed issues first; your question may already be answered.

## Credits

- **Phazerville Suite** — Nicholas J. Michalek (djphazer) and contributors
- **Hemisphere Suite** — Jason Justian and the Benisphere lineage
- **Ornament & Crime** — Patrick Dowling, Max Stadler, Tim Churches
- **Mutable Instruments** — Émilie Gillet, for the vendored DSP
- **Abyss, the display suite** — Calsynth

## Licence

Inherited from upstream, every file header kept. In upstream's words: *"Except
where otherwise noted in file headers, all code herein is generally considered
MIT licensed. However, there are some GPLv3 bits included, so the whole thing is
also subject to compliance with the GPL."*
