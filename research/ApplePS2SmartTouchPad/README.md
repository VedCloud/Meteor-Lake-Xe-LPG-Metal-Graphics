# ApplePS2SmartTouchPad

macOS kernel extension (kext) providing **advanced PS/2 touchpad and keyboard support** with multi-touch gesture capabilities for Hackintosh laptops.

---

## Overview

ApplePS2SmartTouchPad is a custom PS/2 driver that enables enhanced touchpad functionality on laptops where native macOS support is unavailable.

It extends standard PS/2 input handling by introducing gesture recognition, multi-touch emulation, and vendor-specific optimizations for devices such as Elan, Synaptics, and Alps touchpads.

---

## Features

- Multi-touch gesture support (2, 3, and 4 finger gestures)  
- Tap-to-click and physical click handling  
- Two-finger scrolling and gesture mapping  
- Keyboard + touchpad unified PS/2 stack  
- Vendor-specific handling (Elan / Synaptics / Alps)  
- Configurable behavior via plist (gesture mapping, sensitivity, etc.)  

---

## Documentation

📘 For detailed configuration, supported gestures, tuning options, and advanced usage, refer to:

- **GUIDEME.md**  
- **GUIDE.rtf**

---

## Technical Details

- Language: C++  
- Framework: macOS IOKit  
- Interfaces: PS/2 Controller (keyboard + touchpad)  
- Type: Kernel Extension (kext)  
- Function: Gesture processing and input event translation  

---

## Installation

1. Copy `ApplePS2SmartTouchPad.kext` to:
   - `/Library/Extensions/`  
   or  
   - EFI (`Clover` / `OpenCore`)

2. Rebuild cache: `sudo kextcache -i /`  
3. Reboot  

> ⚠️ Do not use alongside conflicting PS/2 kexts (e.g., VoodooPS2Controller)

---

## Compatibility

- macOS (Hackintosh only)  
- Laptops with PS/2-based touchpads  
- Commonly works with:
  - Elan  
  - Synaptics  
  - Alps  

> Note: Feature support depends on hardware and configuration.

---

## Limitations

- PS/2 hardware has bandwidth limitations compared to modern interfaces (I2C)  
- Gesture support may vary across devices  
- Requires tuning via configuration files for optimal behavior  

---

## Status

⚠️ Legacy project – developed for older Hackintosh systems.

---

## Credits

- Hackintosh community  
- Prior PS/2 driver implementations and research  

---

## Disclaimer

This project is intended for educational and experimental purposes.

- Not affiliated with Apple or hardware vendors  
- Use at your own risk  

---

## License

This project is licensed under the **GNU General Public License v2.0 (GPL-2.0)**.
