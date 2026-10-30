#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
xcrun clang -arch arm64 -isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX27.sdk -mmacosx-version-min=27.0 -fobjc-arc -fblocks -O1 -Wall -Wextra "$@" dell27-display-bridge.m -framework Foundation -framework CoreGraphics -framework IOKit -framework IOSurface -framework CoreVideo -o dell27-display-bridge
codesign --force --sign - dell27-display-bridge
codesign --verify --strict dell27-display-bridge
