# Candidate 0.3.16: gamma/Night Shift path and ALC295 audio

Includes the complete0.3.15 publication, wallpaper and brightness-key candidate. Metal plugin0.3.15, GPU kernel1.0.15, ANV4GiB backend, transport, touchpad and About This Mac assets are unchanged. New framebuffer0.2.8 and accelerator0.2.7 implement the colour path. Brightness keys/wallpaper/Weather fixes from0.3.15 still require target observation; no full hardware rendering suite is rerun.

## Gamma and Night Shift

Both previous framebuffer setGammaTable overloads returned success without applying their input. Framebuffer0.2.8 now validates and stores the macOS table, accepts1/3 channels,2..1024 entries and1..16-bit integer samples, and converts it to a256-entry RGB table. Explicit all-zero/null reset restores identity; malformed requests are rejected. Tables are protected by a lock, and content changes advance a generation number. Gamma width16/count256 are advertised. The physical BGRA8 copy path applies the RGB table while preserving alpha, padding and the original source pixels. No PWM, display timing, Intel hardware LUT/modeset registers, private CoreBrightness frameworks or Night Shift eligibility checks are patched.

Identity gamma keeps the existing REP MOVSB/SFENCE bulk copy. Nonidentity gamma uses a scalar per-pixel CPU lookup during the final scanout copy, so it has additional CPU cost. This is a functional colour-output candidate, not a performance improvement. The existing completion/idle check refreshes a stationary completed front surface once when the gamma generation changes; an incomplete GPU publication still keeps the previous physical picture. Same-source/destination requests are not transformed in place, preventing cumulative colour changes; a refusal counter records that unsupported case. The last report confirmed this Dell's current path copies from a distinct IOSurface, but target validation remains required. Idle observation can add up to about200ms after the active retry window. This does not establish vblank synchronization or sleep/wake support.

Night Shift's UI/colour schedule must be tested on the Dell. The feature is not marked working merely because gamma callbacks compile: registry diagnostics record table count, width, active state and generation, and accelerator counters record actual nonidentity copies/refused paths. If Night Shift does not call the gamma API, those diagnostics distinguish that from a failed final transform. No user colour preferences or schedule are changed on the build host or by the installer.

## Audio candidate

The user confirmed Realtek ALC295. The Linux PCI report identifies Intel8086:7e28, Dell1028:0cb6 at00:1f.3, but its audio/cards.txt says no soundcards and contains no codec/pin dump. A confirmed codec family does not establish the exact speaker/headphone/microphone pin map.

The USB adds upstream AppleALC1.9.8 with existing Lilu1.7.3. The official release ZIP SHA256 is d4d36cd2da7863cf7cbfcb0892d45c08457f4f4ddd065a073dd010e46bd63bcb and matches GitHub's asset digest. Its x86_64 slice is retained and ad-hoc signed; the unused i386 object slice is omitted. AppleALC source/resources are not modified. The upstream license accompanies the bundle.

AppleALC's current controller resource list lacks7e28 but includes Raptor Lake mobile51CA. The audio PCI path PciRoot(0x0)/Pci(0x1F,0x3) therefore receives device-id Data ca510000 as a compatibility identity, plus layout-id and alc-layout-id Data21000000 (decimal33). Layout33 is upstream's ALC295/ALC3254 profile for Dell Latitude7210 2-in-1, used as an initial related-Dell routing candidate. It is not asserted to be the5350's final correct pin map. This spoof affects the macOS registry/driver matching; it does not alter physical PCI hardware identity. No GPU identity, IRQ map, device hiding, global boot arguments or AppleHDA system files are edited. The only EFI differences from0.3.15 are the audio properties, the enabled AppleALC entry after Lilu and its kext files.

This attempts native analog HDA speakers/headphones. Actual sound, jack detection, capture and amplifier routing are unverified. Intel SST/DMIC input and HDMI/DisplayPort audio are not claimed working. The next collector records loaded drivers, AppleHDA controller/codec/IOAudioDevice registry state and a bounded CoreAudio device listing. Playback and recording require target tests; no volume changes or test tones are triggered automatically.

## Verification and backup

Framebuffer/accelerator compile for x86_64 and signatures verify. ASan/UBSan checks exercise the real gamma decoder/copy (integer sample widths, identity ramps, warm RGB output, alpha/padding, malformed and in-place refusal), actual framebuffer setter (generation/reset/error handling) and actual front timer (one redraw on an idle gamma change). Prior publication tests remain valid because that code is unchanged. Installer/rollback and collector fixtures pass. Public new imports are IOService::isInactive in the accelerator and libkern memcmp in the framebuffer; the corresponding existing IOKit/libkern dependencies remain. Target AuxiliaryKC resolution is still required.

The audio plan is checked against upstream codec/controller resources, official release digest and existing Lilu; the modified config passes official ocvalidate1.0.8. No bootloader upgrade is performed. The previous complete0.3.15 EFI and replaced package files are backed up under backups/nightshift-audio-before-0.3.16 on the USB and locally. This is not a Dell OS/AuxKC snapshot. The Dell installer separately backs up installed kexts and AuxKC and retains one-shot fallback.

## Dell trial

Boot the fallback desktop through this USB, then run:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

After exit=0, reboot through USB. Test Night Shift manually with its warmth slider, Control Centre brightness and brightness keys, then test speakers/headphones at a modest chosen volume. Check Sound settings for available outputs and inputs. Also check wallpaper redraw and Weather from the preceding candidate. Collect on that active boot:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/collect-desktop-failure.sh
```

The logs distinguish gamma callbacks/copies from UI-only changes and audio codec attachment from a merely loaded AppleALC kext. Successful installation alone does not confirm either feature.

## Primary references

- [Apple IOFramebuffer gamma validation/update path](https://github.com/apple-oss-distributions/IOGraphics/blob/main/IOGraphicsFamily/IOFramebuffer.cpp)
- [AppleALC1.9.8 official release](https://github.com/acidanthera/AppleALC/releases/tag/1.9.8)
- [AppleALC ALC295 layouts](https://github.com/acidanthera/AppleALC/blob/master/Resources/ALC295/Info.plist)
- [AppleALC supported controller patches](https://github.com/acidanthera/AppleALC/blob/master/Resources/Controllers.plist)
- [AppleALC controller discovery and property handling](https://github.com/acidanthera/AppleALC/blob/master/AppleALC/kern_alc.cpp)
