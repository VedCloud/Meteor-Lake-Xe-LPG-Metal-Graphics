#!/bin/bash
# build.sh <out>: NMXVGAFB.kext (run on the Mac / in the VM)
set -uo pipefail
D=$(cd "$(dirname "$0")" && pwd); OUT=$1; mkdir -p "$OUT/NMXVGAFB.kext/Contents/MacOS"
SDK=$(xcrun --show-sdk-path); KHDR="$SDK/System/Library/Frameworks/Kernel.framework/Headers"
xcrun clang++ -arch x86_64 -fapple-kext -mkernel -nostdinc -I"$KHDR" -DKERNEL -DKERNEL_PRIVATE -DDRIVER_PRIVATE -DAPPLE -DNeXT \
  -std=c++17 -fno-rtti -fno-exceptions -fno-builtin -fno-common -Wall -Wno-unused-parameter -Wno-inconsistent-missing-override -O2 \
  -c "$D/nmxvgafb.cpp" -o "$OUT/nmxvgafb.o" || exit 1
xcrun clang++ -arch x86_64 -fapple-kext -mkernel -nostdinc -I"$KHDR" -DKERNEL -DKERNEL_PRIVATE -DDRIVER_PRIVATE -DAPPLE -DNeXT \
  -std=c++17 -fno-rtti -fno-exceptions -fno-builtin -fno-common -Wall -Wno-unused-parameter -Wno-inconsistent-missing-override -O2 \
  -c "$D/nmx_virtio_gpu.cpp" -o "$OUT/nmx_virtio_gpu.o" || exit 1
xcrun clang++ -arch x86_64 -fapple-kext -nostdlib -Xlinker -kext -lkmodc++ -lkmod -lcc_kext "$OUT/nmxvgafb.o" "$OUT/nmx_virtio_gpu.o" \
  -o "$OUT/NMXVGAFB.kext/Contents/MacOS/NMXVGAFB" || exit 1
cp "$D/Info.plist" "$OUT/NMXVGAFB.kext/Contents/Info.plist"
echo "NMXVGAFB $(stat -f %z "$OUT/NMXVGAFB.kext/Contents/MacOS/NMXVGAFB") B"
