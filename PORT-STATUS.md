> **Current package publication — 30 October 2026:** [Complete Tahoe 0.3.21 graphics/audio, matching Wi-Fi and source packages, and the macOS27 full-engine diagnostic](packages/) are now published. Historical entries below retain their original dates and test scopes. macOS27 has reached native WindowServer startup; its graphical installer is not verified.

# Latitude 5350 port status — 18 October 2026

The PCI probe succeeded on the Dell's real 8086:7D45 GPU under Sequoia 15.8.1 (24H32). The older MyIntelGPU on the laptop failed authentication; its signature error is separate from Meteor Lake support.

The USB now includes an experimental acceleration runtime at iGPU-Port/Acceleration. The rebuilt parts are NMXGPU (438 i915/DRM/runtime sources), NMXAccel, NVMTLDriver, AIR-to-SPIR-V translator, libdrm, and ANV (1,045 build steps). Package hashes and ad-hoc kext/plugin signatures verify after copying to the USB. This runtime is still untested on the Dell and has not been installed on the build Mac.

SIP configuration on the USB is now Data 430A0000, with an NVRAM/Delete entry so the old value is replaced on boot. SecureBootModel is Disabled, AMFIPass 1.4.1 is enabled after Lilu, and the AMFI/ipc arguments required by the supplied NullMoth setup are present. These settings take effect on the next boot through this USB. No persistent nmxintel activation was added.

Next: boot the Dell through this USB, run Acceleration/install-runtime.sh with sudo, reboot through the same USB if installation succeeds, then run Acceleration/collect-runtime.sh with sudo. The installer validates the candidate against the Dell's kernel collections before live driver changes. It backs up the old runtime, arms one test boot, and does not reboot automatically. Reports are saved directly to Acceleration/reports on this USB.

See [the acceleration package instructions](latitude-runtime/README.md) for exact commands, rollback and limitations. Vulkan and Metal output verification are included. Application rendering/compute acceleration is the first hardware milestone. Full GPU composition of WindowServer on the physical panel remains unfinished; this candidate's Metal plugin intentionally declines WindowServer on its headless physical-display path. Hardware video decode, sleep/wake and stability are also unverified. Do not interpret the build as completed acceleration support.

The pre-security-change config is backed up at backups/20261018-133420-before-sip-change/config.plist. A full earlier EFI backup remains on the build Mac under /Users/ved/Documents/iGPU Port/backups/20261018-131642-before-probe/EFI. Additional prerequisite changes have their own before-runtime-prerequisites config backup on this USB. Original diagnostic source and evidence remain under iGPU-Port and MyIntelGPU/ports/latitude5350.
