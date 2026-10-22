Current graphics candidate0.3.12 also includes read-only branding diagnostics in collect-desktop-failure.sh. They compare active icon hashes, snapshot identities and ImageIO decode results. The existing branding transaction and backup naming remain0.3.11.

# Dell branding — bundled with graphics 0.3.11

Intended About This Mac appearance:

- Dell Latitude 5350 2-in-1
- 13-inch, 2024
- The supplied transparent Dell image (ICNS copied byte for byte; PNG included for reference).

Run the usual installer from the Dell fallback desktop:

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/install-windowserver.sh
```

The graphics installer completes its existing commit and one-shot arming, then runs `Desktop/install-about-this-mac.sh`. Branding is a separate transaction. If it fails, the overall command returns **4**, explicitly says graphics are installed/armed, and saves the error to `Desktop/reports/windowserver-install-*/about-this-mac.txt`. It does not roll back working graphics or rerun the rendering suite. No automatic reboot.

The native helper targets only Intel Sequoia 24H32, MacBookPro16,2 SMBIOS and the visible 7D45 GPU. It reads the caller's serial-derived cache key and locale, preserves unrelated System Profiler entries, and writes only CPU Names/ModelNames through that user's preference service. The displayed marketing name is deliberately customized; the OpenCore SMBIOS stays MacBookPro16,2.

For the icon, it resolves CoreTypes declarations and inherited/color variants for that model, identifies the System volume by APFS container, role and volume-group identity, validates the build and active icon bytes, backs up originals, replaces the selected icon files, and creates a boot snapshot with bless. It requires authenticated-root already disabled; it never changes SIP, resets NVRAM or removes global icon caches. The new snapshot is a real System-volume change, not just an image placed on the USB. macOS updates may restore Apple's icon.

Backups live on the Dell at `/Library/Application Support/LatitudeBranding/0.3.11-<UUID>/`. They include original icon bytes, the bundled icon for transaction rollback, previous preference values, previous root snapshot identity, and the helper. `latest.plist` points to the latest successful transaction. Reinstall after a successful boot reuses that original backup when the installed icon already matches.

Independent cosmetic restore, on the Dell (then reboot when ready):

```sh
sudo /bin/bash /Volumes/SEQOUIA/iGPU-Port/Desktop/restore-about-this-mac.sh
```

An explicit backup directory can be supplied as the final quoted argument. Restore checks the system volume/build and installed icon hash; it refuses to overwrite icons changed by another update. It restores only preference entries altered by this package, preserving later unrelated entries. It creates a new snapshot containing the original icons, rather than rolling the entire OS back to an old snapshot. It does not change or arm graphics.

Local verification: universal native helper compiles with warnings as errors; native arm64 and x86_64/Rosetta read-only fixtures pass for preference merge/restoration, absent-key restoration, APFS discovery/refusal and CoreTypes inherited icon resolution. ICNS decodes with iconutil. Graphics installer fixtures include cosmetic-failure isolation. Actual System-volume writes/bless and About This Mac layout/cache refresh have not been run on the Dell. If macOS rewrites its model cache or retains an old icon, collect the installer report before changing anything else; there is no automatic NVRAM reset or repeated background preference enforcement.

References: [user-supplied guide](https://www.reddit.com/r/hackintosh/comments/1iubdhk/guide_to_change_about_this_mac_name_and_photo_on/), [CoreTypes model/icon discovery](https://github.com/bkahlert/device-icons), [Apple SSV design](https://support.apple.com/guide/security/signed-system-volume-security-secd698747c9/web). Guide steps involving guessed disks, SIP re-enabling and NVRAM resets were not adopted.
