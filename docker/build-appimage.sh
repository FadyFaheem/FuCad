#!/usr/bin/env bash
set -euo pipefail

SRC_DIR="${SRC_DIR:-/src}"
cd "$SRC_DIR"

# The bind-mounted checkout is owned by the host user; reuse that identity so
# anything we create stays writable by them afterwards.
HOST_UID="${HOST_UID:-$(stat -c '%u' "$SRC_DIR")}"
HOST_GID="${HOST_GID:-$(stat -c '%g' "$SRC_DIR")}"

# The checkout is bind-mounted from the host, so git refuses to operate on it
# until it is marked safe.
git config --global --add safe.directory "$SRC_DIR"

if [ -f .gitmodules ]; then
    if git submodule status --recursive 2>/dev/null | grep -q '^-'; then
        echo "==> Initializing git submodules"
        git submodule update --init --recursive
        while read -r _ path; do
            [ -n "$path" ] && chown -R "$HOST_UID:$HOST_GID" "$SRC_DIR/$path"
        done < <(git config --file .gitmodules --get-regexp '\.path$')
        chown -R "$HOST_UID:$HOST_GID" "$SRC_DIR/.git/modules" 2>/dev/null || true
    fi
fi

export BUILD_TAG="${BUILD_TAG:-local}"
export UPLOAD_RELEASE=false
# appimagetool is itself an AppImage; let it run without FUSE inside Docker.
export APPIMAGE_EXTRACT_AND_RUN=1
export CMAKE_BUILD_PARALLEL_LEVEL="${CMAKE_BUILD_PARALLEL_LEVEL:-4}"

cd package/rattler-build

# Re-resolve the lock so recipe changes (e.g. added host dependencies) are
# picked up even when the repository ships an older pixi.lock.
echo "==> Resolving pixi lock file"
pixi lock

echo "==> Installing pixi environments (this compiles FuCad)"
pixi install --frozen

echo "==> Building and bundling the AppImage"
pixi run -e package create_bundle

echo "==> Collecting artifacts into $SRC_DIR/dist"
mkdir -p "$SRC_DIR/dist"
shopt -s nullglob
for artifact in linux/*.AppImage linux/*.AppImage.zsync linux/*-SHA256.txt; do
    cp -v "$artifact" "$SRC_DIR/dist/"
done
shopt -u nullglob

# Keep the generated files usable by the host user instead of root.
chown -R "$HOST_UID:$HOST_GID" \
    "$SRC_DIR/dist" \
    "$SRC_DIR/package/rattler-build/pixi.lock" \
    "$SRC_DIR/package/rattler-build/linux" 2>/dev/null || true

echo "==> Done"
ls -lh "$SRC_DIR/dist"
