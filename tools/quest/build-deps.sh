#!/usr/bin/env bash
# Build the Quest (Android arm64) dependencies for OpenXWA-VR:
#   - host tools: aeron_fsr3_shadergen (must run on the build PC)
#   - SDL3 (main branch, for SDL_gpu OpenXR support), shared
#   - zstd, static
#   - FFmpeg, static, only the SMUSH/SANM/VIMA pieces the game needs
# Everything lands under build/quest/ (git-ignored). Re-runnable: finished
# steps are skipped unless FORCE=1.
#
# Requires the host shadercross from ~/tools/xwa-tools (see OpenXWA docs).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OUT="$ROOT/build/quest"
SRC="$OUT/src"
PREFIX="$OUT/deps"
HOST="$OUT/host-tools"

NDK="${ANDROID_NDK_ROOT:-$HOME/Android/Sdk/ndk/27.2.12479018}"
API=29
ABI=arm64-v8a
TC="$NDK/toolchains/llvm/prebuilt/linux-x86_64"
JOBS="$(nproc)"

SDL_COMMIT=ce3af25d4
ZSTD_VERSION=1.5.7
FFMPEG_VERSION=8.0.1

XWA_TOOLS="$HOME/tools/xwa-tools"
export LD_LIBRARY_PATH="$XWA_TOOLS/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

ANDROID_CMAKE=(
    -G Ninja
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_TOOLCHAIN_FILE="$NDK/build/cmake/android.toolchain.cmake"
    -DANDROID_ABI="$ABI"
    -DANDROID_PLATFORM="android-$API"
    -DCMAKE_INSTALL_PREFIX="$PREFIX"
    -DCMAKE_PREFIX_PATH="$PREFIX"
    -DCMAKE_FIND_ROOT_PATH="$PREFIX"
)

mkdir -p "$SRC" "$PREFIX" "$HOST"

done_marker() { [[ "${FORCE:-0}" != 1 && -f "$OUT/.done-$1" ]]; }
mark_done() { touch "$OUT/.done-$1"; }

# ---------------------------------------------------------------- host tools
if ! done_marker host-shadergen; then
    echo "== host: aeron_fsr3_shadergen"
    cmake -S "$ROOT/aeron" -B "$OUT/build-host-tools" -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DAERON_HOST_TOOLS_ONLY=ON \
        -DCMAKE_INSTALL_PREFIX="$HOST"
    cmake --build "$OUT/build-host-tools" --parallel "$JOBS"
    cmake --install "$OUT/build-host-tools"
    mark_done host-shadergen
fi

# ---------------------------------------------------------------------- SDL3
if ! done_marker sdl3; then
    echo "== SDL3 @ $SDL_COMMIT"
    [[ -d "$SRC/SDL/.git" ]] || git clone https://github.com/libsdl-org/SDL.git "$SRC/SDL"
    git -C "$SRC/SDL" fetch --quiet origin || true
    git -C "$SRC/SDL" checkout --quiet "$SDL_COMMIT"
    cmake -S "$SRC/SDL" -B "$OUT/build-sdl3" "${ANDROID_CMAKE[@]}" \
        -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF \
        -DSDL_INSTALL_DOCS=OFF
    cmake --build "$OUT/build-sdl3" --parallel "$JOBS"
    cmake --install "$OUT/build-sdl3"
    mark_done sdl3
fi

# ---------------------------------------------------------------------- zstd
if ! done_marker zstd; then
    echo "== zstd $ZSTD_VERSION"
    [[ -d "$SRC/zstd" ]] || git clone --quiet --branch "v$ZSTD_VERSION" --depth 1 \
        https://github.com/facebook/zstd.git "$SRC/zstd"
    cmake -S "$SRC/zstd/build/cmake" -B "$OUT/build-zstd" "${ANDROID_CMAKE[@]}" \
        -DZSTD_BUILD_PROGRAMS=OFF -DZSTD_BUILD_SHARED=OFF -DZSTD_BUILD_STATIC=ON \
        -DZSTD_BUILD_TESTS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    cmake --build "$OUT/build-zstd" --parallel "$JOBS"
    cmake --install "$OUT/build-zstd"
    mark_done zstd
fi

# -------------------------------------------------------------------- FFmpeg
if ! done_marker ffmpeg; then
    echo "== FFmpeg $FFMPEG_VERSION"
    [[ -d "$SRC/FFmpeg" ]] || git clone --quiet --branch "n$FFMPEG_VERSION" --depth 1 \
        https://github.com/FFmpeg/FFmpeg.git "$SRC/FFmpeg"
    rm -rf "$OUT/build-ffmpeg" && mkdir -p "$OUT/build-ffmpeg"
    (
        cd "$OUT/build-ffmpeg"
        "$SRC/FFmpeg/configure" \
            --prefix="$PREFIX" \
            --target-os=android \
            --arch=aarch64 \
            --cpu=armv8-a \
            --enable-cross-compile \
            --sysroot="$TC/sysroot" \
            --cc="$TC/bin/aarch64-linux-android$API-clang" \
            --cxx="$TC/bin/aarch64-linux-android$API-clang++" \
            --ar="$TC/bin/llvm-ar" \
            --nm="$TC/bin/llvm-nm" \
            --ranlib="$TC/bin/llvm-ranlib" \
            --strip="$TC/bin/llvm-strip" \
            --pkg-config=false \
            --disable-autodetect \
            --disable-avdevice \
            --disable-avfilter \
            --disable-debug \
            --disable-doc \
            --disable-everything \
            --disable-network \
            --disable-programs \
            --disable-shared \
            --enable-static \
            --enable-pic \
            --enable-pthreads \
            --enable-decoder=adpcm_vima \
            --enable-decoder=sanm \
            --enable-demuxer=smush
        make -j"$JOBS"
        make install
    )
    mark_done ffmpeg
fi

echo "== deps ready in $PREFIX"
ls "$PREFIX/lib"
