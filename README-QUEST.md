# OpenXWA VR for Meta Quest

Standalone Meta Quest 3 port of OpenXWA (Star Wars: X-Wing Alliance) with
OpenXR: menus on a virtual screen, a head-tracked stereo
3D cockpit in flight, and Touch controller support. The X-Wing, Y-Wing,
A-Wing, B-Wing, TIE Fighter/Interceptor, TIE Bomber and Millennium Falcon
use the more detailed 3D cockpits from X-Wing Revival (included; their
small built-in screens are dark); all other craft keep X-Wing Alliance's
own cockpits.

You need your own copy of X-Wing Alliance (Steam or GOG). The APK contains
no game data.

## Install

1. Install the APK with SideQuest or `adb install OpenXWA-VR-*.apk`.
2. Copy the whole X-Wing Alliance game folder (the one containing
   `STRINGS.TXT`, `FRONTRES`, `MISSIONS`, ...) to the Quest's internal
   storage as `OpenXWA`, so that `/sdcard/OpenXWA/STRINGS.TXT` exists.
3. Start "OpenXWA VR" from Unknown Sources. On first start, allow
   "access to all files" when Android asks (or run
   `adb shell appops set --uid com.bilko.openxwavr MANAGE_EXTERNAL_STORAGE allow`).

Optional: to make hyperspace selectable in Options > Game Controller
Options, append this line to `/sdcard/OpenXWA/JOYSTICK.TXT`:

    !KEY_H!104 H Hyperspace

## Controls

Hold the Meta button on the right controller to re-centre the view.

Menus (concourse, briefing, options, hangar, in-flight Escape menu):
point with either controller, trigger = click, right grip = right-click,
A = Enter, B / Menu = Escape, Y = Space, left stick = arrow keys,
left trigger = launch from the hangar.

Typing (pilot names, save names): whenever the game waits for text, a
virtual keyboard appears below the screen. Point at a key with either
controller and pull the trigger; Shift is on for the first letter, holding
Back keeps deleting, Enter (or A) confirms.

Flight (the Touch controllers act as one gamepad; every button can be
remapped in Options > Game Controller Options):

| Input | Action |
|---|---|
| Left stick | Pitch and yaw |
| Right stick left/right | Roll |
| Right stick up/down | Throttle lever (stays where you leave it) |
| Right trigger | Fire |
| Left trigger | Countermeasure |
| Right grip | Pick target in sight |
| A / B | Next target / fighters targeting you |
| X / Y | Cycle weapons / cycle firing mode (link fire) |
| Left stick click | Nearest fighter |
| Right stick click | Match target speed |
| Menu | Options screen |
| Left grip (hold) | Command menu |
| Left grip + left stick click | Hyperspace: first click asks, second click jumps (at a hyper buoy one click jumps) |

Command menu: hold the left grip and a panel with the remaining flight
commands appears below the view; point at an entry with either controller
and pull its trigger (the triggers don't fire while the panel is open).
Releasing the grip closes it. The game keeps running meanwhile.

| Energy | Targeting | Wingmen | Ship |
|---|---|---|---|
| Laser rate (F9) | Next objective (O) | Attack target (Shift-A) | Pick up cargo (Shift-P) |
| Shield rate (F10) | Components (,) | Cover me (Shift-C) | Release cargo (Shift-R) |
| Shield balance (S) | Its attacker (A) | Ignore target (Shift-I) | Dock / board (Shift-D) |
| Shield > laser (Shift-F9) | Map (M) | Reinforce (Shift-S) | S-foils (V) |
| Laser > shield (Shift-F10) | Hyperspace (H) | Call resupply (Shift-B) | Confirm (Space) |

## Build (Linux host)

Requirements: Android SDK + NDK 27.2.12479018, JDK 17, SDL_shadercross
(see the Dockerfile recipe), CMake, Ninja.

    git submodule update --init
    tools/quest/build-deps.sh      # SDL3, zstd, FFmpeg, host shader tools
    tools/quest/build-native.sh    # libmain.so + runtime resources
    (cd android && ./gradlew assembleDebug)
    tools/quest/deploy.sh          # install, copy game data once, launch

VR code lives in `src/xwa_vr/` (OpenXR session, input, layers),
`src/xwa_remaster/vr_stereo.h` + `flight.c` (per-eye rendering) and the
`quest-vr` branch of the Aeron engine fork (`external_present.*`).

X-Wing Revival cockpits are converted at build time from Revival's free
game data by `tools/quest/gen_revival_cockpits.py` (set `XW_REVIVAL_DATA`
to its `gamedata` folder).

## Credits

3D cockpit models from [X-Wing Revival](https://www.raptor007.com/xwing/)
by Raptor007 and its contributors (originally from the X-Wing Alliance
Upgrade project).
