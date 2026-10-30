# Current Dell packages — 30 October 2026

The latest packages are downloadable from the [dated release](https://github.com/VedCloud/Meteor-Lake-Xe-LPG-Metal-Graphics/releases/tag/dell-packages-20261030).
These are actual payloads and matching source, not just README version changes.

| Download | Size |
|---|---|
| [Tahoe-Graphics-Audio-0.3.21-20261030.tar.gz](https://github.com/VedCloud/Meteor-Lake-Xe-LPG-Metal-Graphics/releases/download/dell-packages-20261030/Tahoe-Graphics-Audio-0.3.21-20261030.tar.gz) | 91.0 MiB |
| [Tahoe-Network-0.1.2-20261030.tar.gz](https://github.com/VedCloud/Meteor-Lake-Xe-LPG-Metal-Graphics/releases/download/dell-packages-20261030/Tahoe-Network-0.1.2-20261030.tar.gz) | 15.2 MiB |
| [Tahoe-Corresponding-Sources-20261030.tar.gz](https://github.com/VedCloud/Meteor-Lake-Xe-LPG-Metal-Graphics/releases/download/dell-packages-20261030/Tahoe-Corresponding-Sources-20261030.tar.gz) | 45.3 MiB |
| [macOS27-UEFI-Engine-20261030.tar.gz](https://github.com/VedCloud/Meteor-Lake-Xe-LPG-Metal-Graphics/releases/download/dell-packages-20261030/macOS27-UEFI-Engine-20261030.tar.gz) | 1.1 MiB |

Verify downloads against [SHA256SUMS.txt](SHA256SUMS.txt).
[INDEX.json](INDEX.json) records versions, exact hashes and validation scope.

- **Tahoe graphics/audio:** full 0.3.21 runtime, translator, shader tools including
  libLLVM, NMXGPU1.0.15, NMXAccel0.3.1, framebuffer0.2.12 and SimpleHDA1.0.0,
  Dell branding, prepared-USB guarded installers, rollback and build evidence.
  Its original 163-file manifest passed before packaging. Large compiler files
  belong in the complete release asset rather than a Git checkout with missing files.
- **Tahoe Wi-Fi:** private revision0.1.2, byte-identical to the confirmed checkpoint.
  Browsing/installer Wi-Fi works; AirDrop is not functional.
- **Corresponding Tahoe sources:** graphics/plugin/translator, private bridge and
  accelerator changes, complete network source including its large generated
  firmware array, SimpleHDA and later input experiments. The checkout in
  [tahoe-port](../../tahoe-port/) omits only that generated array; the archive includes it.
  Original local build paths are recorded, not advertised as portable builders.
- **macOS27:** current diagnostic EFI, bridge/device/profile sources, exact upstream
  overlay and build/admission evidence. Native kernel, recovery services and
  WindowServer startup are reached. The graphical installer remains unverified.
  Apple OS images/initialized-memory captures and SDK/KDK payloads are not included.

The original Sequoia packages, upstream dependencies and frozen passing reports
remain historical and are not overwritten with Tahoe-incompatible binaries merely
to change their GitHub timestamps. New Tahoe code is under tahoe-port; the full
macOS27 engine is under macos27-standalone/full-engine.

The source snapshot includes the latest 1.0.10 touchpad experiment and its recorded
runtime failure, plus pen1.0.6 work. These are **not promoted as fixed input drivers**.
The user's quiet multiboot EFI and normal input configuration are not replaced by
these archives. The graphics installer retains its exact prepared-USB and family
checks; do not bypass them or treat this as a generic installer.
