#!/usr/bin/env bash
set -euo pipefail

APP_NAME="RYCOM"
APP_VERSION="2.6.5"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build-codex/macos"
OUTPUT_DIR="$ROOT_DIR/build-codex"
APP_BUNDLE="$BUILD_DIR/$APP_NAME.app"
DMG_OUTPUT="$OUTPUT_DIR/$APP_NAME-$APP_VERSION.dmg"

find_qt_bin() {
    if [[ -n "${QT_BIN:-}" ]]; then
        echo "$QT_BIN"
        return 0
    fi

    local candidates=(
        "$HOME/Qt/6.9.2/macos/bin"
        "$HOME/Qt/6.9.2/clang_64/bin"
        "/Applications/Qt/6.9.2/macos/bin"
        "/Applications/Qt/6.9.2/clang_64/bin"
    )

    local candidate
    for candidate in "${candidates[@]}"; do
        if [[ -x "$candidate/qmake" && -x "$candidate/macdeployqt" ]]; then
            echo "$candidate"
            return 0
        fi
    done

    local qmake_path
    qmake_path="$(command -v qmake || true)"
    if [[ -n "$qmake_path" ]]; then
        local bin_dir
        bin_dir="$(cd "$(dirname "$qmake_path")" && pwd)"
        if [[ -x "$bin_dir/macdeployqt" ]]; then
            echo "$bin_dir"
            return 0
        fi
    fi

    return 1
}

if [[ "$(uname -s)" != "Darwin" ]]; then
    echo "This script must be run on macOS. A valid DMG requires macdeployqt and hdiutil." >&2
    exit 1
fi

QT_BIN="$(find_qt_bin)" || {
    echo "Cannot find Qt bin directory. Set QT_BIN, for example:" >&2
    echo "  QT_BIN=\"\$HOME/Qt/6.9.2/macos/bin\" ./scripts/package_macos_dmg.sh" >&2
    exit 1
}

QMAKE="${QMAKE:-$QT_BIN/qmake}"
MACDEPLOYQT="${MACDEPLOYQT:-$QT_BIN/macdeployqt}"

command -v hdiutil >/dev/null 2>&1 || {
    echo "hdiutil not found. Run this script on a normal macOS system." >&2
    exit 1
}

if [[ ! -x "$QMAKE" ]]; then
    echo "qmake not found or not executable: $QMAKE" >&2
    exit 1
fi

if [[ ! -x "$MACDEPLOYQT" ]]; then
    echo "macdeployqt not found or not executable: $MACDEPLOYQT" >&2
    exit 1
fi

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR" "$OUTPUT_DIR"

pushd "$BUILD_DIR" >/dev/null
"$QMAKE" "$ROOT_DIR/sources/RYCOM.pro" CONFIG+=release
make -j"$(sysctl -n hw.ncpu)"
popd >/dev/null

if [[ ! -d "$APP_BUNDLE" ]]; then
    echo "Build did not create $APP_BUNDLE" >&2
    exit 1
fi

PLIST="$APP_BUNDLE/Contents/Info.plist"
if [[ -f "$PLIST" ]]; then
    /usr/libexec/PlistBuddy -c "Set :CFBundleShortVersionString $APP_VERSION" "$PLIST" 2>/dev/null \
        || /usr/libexec/PlistBuddy -c "Add :CFBundleShortVersionString string $APP_VERSION" "$PLIST"
    /usr/libexec/PlistBuddy -c "Set :CFBundleVersion $APP_VERSION" "$PLIST" 2>/dev/null \
        || /usr/libexec/PlistBuddy -c "Add :CFBundleVersion string $APP_VERSION" "$PLIST"
    /usr/libexec/PlistBuddy -c "Set :CFBundleIdentifier com.rymcu.RYCOM" "$PLIST" 2>/dev/null \
        || /usr/libexec/PlistBuddy -c "Add :CFBundleIdentifier string com.rymcu.RYCOM" "$PLIST"
    /usr/libexec/PlistBuddy -c "Set :CFBundleIconFile rymculogo" "$PLIST" 2>/dev/null \
        || /usr/libexec/PlistBuddy -c "Add :CFBundleIconFile string rymculogo" "$PLIST"
fi

mkdir -p "$APP_BUNDLE/Contents/Resources"
cp "$ROOT_DIR/sources/rymculogo.icns" "$APP_BUNDLE/Contents/Resources/rymculogo.icns"

DEPLOY_ARGS=("$APP_BUNDLE" -dmg -always-overwrite -verbose=2)
if [[ -n "${CODESIGN_IDENTITY:-}" ]]; then
    DEPLOY_ARGS+=("-codesign=$CODESIGN_IDENTITY")
fi

"$MACDEPLOYQT" "${DEPLOY_ARGS[@]}"

GENERATED_DMG="$BUILD_DIR/$APP_NAME.dmg"
if [[ ! -f "$GENERATED_DMG" ]]; then
    GENERATED_DMG="$(find "$BUILD_DIR" -maxdepth 1 -name "*.dmg" -print -quit)"
fi

if [[ -z "$GENERATED_DMG" || ! -f "$GENERATED_DMG" ]]; then
    echo "macdeployqt finished, but no DMG was found in $BUILD_DIR" >&2
    exit 1
fi

rm -f "$DMG_OUTPUT"
mv "$GENERATED_DMG" "$DMG_OUTPUT"

echo "Created: $DMG_OUTPUT"
