# The Quad Viewer

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

## Keyboard shortcuts

Once connected:

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

## If nothing shows up

- No port in the list: charge-only cable, or the module is in slot X or Y (stock
  Phazerville). Hold **Z** at power-up for the boot menu, then press **A** or **B** (keep
  Z held to save the choice).
- Port is there but the screen stays blank: click Disconnect, power-cycle the module,
  Connect again. Another program holding the port (Arduino IDE serial monitor, a DAW's
  MIDI-over-serial bridge) will also do this — close it.
- Browser says Web Serial isn't available: you are in Safari, or a Firefox older than 151.
  Use Chrome, Edge or Arc.
- 4-up view is empty but the mirror works: the module is not in Quadrants. Click **MODE**.
- USB audio garbles in either direction: set the DAW's I/O buffer to 128 samples or more,
  then deselect and reselect OCOC as the audio device, or unplug and reconnect the USB
  cable.
- The module rebooted on its own: it keeps a report of the last crash. With the viewer
  disconnected, send the four characters `O?!!` to the port in one go (paste them — typed
  one key at a time they arrive too slowly). Put what it prints before `ENDCRASH` in a bug
  report; `NOCRASH` means nothing was recorded. On a Mac, in Terminal:

  ```
  PORT=$(ls /dev/cu.usbmodem* | head -1); stty -f $PORT raw -echo
  cat $PORT & sleep 1; printf 'O?!!' > $PORT; sleep 2; kill %1
  ```

## Building your own

The wire protocol is documented in `PROTOCOL.md` in this repo if you want to build your
own viewer.
