#!/usr/bin/env bash
# Install OpenXWA-VR on the connected Quest, copy the original game data once,
# grant all-files access and launch. Usage:
#   tools/quest/deploy.sh            # install + launch
#   PUSH_DATA=1 tools/quest/deploy.sh  # also (re)copy the game data
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
ADB="${ADB:-$HOME/Android/Sdk/platform-tools/adb}"
PKG=com.bilko.openxwavr
APK="${APK:-$ROOT/android/app/build/outputs/apk/debug/app-debug.apk}"
GAME_DIR="${XWA_GAME_DIR:-$HOME/.steam/debian-installation/steamapps/common/Star Wars X-Wing Alliance}"
DEVICE_DATA=/sdcard/OpenXWA

"$ADB" install -r "$APK"

if [[ "${PUSH_DATA:-0}" == 1 ]] || ! "$ADB" shell "test -f $DEVICE_DATA/STRINGS.TXT"; then
    echo "== copying game data to $DEVICE_DATA (one time, ~830 MB)"
    "$ADB" shell mkdir -p "$DEVICE_DATA"
    "$ADB" push "$GAME_DIR/." "$DEVICE_DATA/"
fi

# The original joystick action list has no hyperspace entry; add H so it can
# be bound in Options > Game Controller Options (the VR default binds it).
if ! "$ADB" shell "grep -q '!KEY_H!104' $DEVICE_DATA/JOYSTICK.TXT"; then
    "$ADB" shell "printf '!KEY_H!104 H Hyperspace\r\n' >> $DEVICE_DATA/JOYSTICK.TXT"
fi

"$ADB" shell appops set --uid "$PKG" MANAGE_EXTERNAL_STORAGE allow
"$ADB" shell am force-stop "$PKG"
"$ADB" logcat -c
"$ADB" shell am start -n "$PKG/.OpenXWAActivity"
