# 0.1.4 native Intel backend checkpoint

Local development build. Not an installable AirDrop release. No USB changes were made for this checkpoint.

The x86_64 Sequoia kext compiled all 74 units, linked, and passed ad-hoc signature verification. Native packet and actual HAL-body fixtures passed AddressSanitizer/UndefinedBehaviorSanitizer with external kernel/firmware primitives substituted. These checks do not validate RF operation or native macOS integration.

Implemented: separately owned secondary PHY/P2P/listener contexts; bounded raw injection queue; native prepared-packet adapter that preserves AWDL sequence and avoids duplicate encapsulation; original-packet ownership separate from the radio DMA copy; firmware response/reclaim separation; reset completion after DMA quiescence; known-peer RX validation and replay rejection. Eleven command/queue failure stages are exercised locally. Uncertain firmware ownership poisons the transaction until reset instead of recycling identifiers.

Not connected: native interface factory/control handlers, native action input metadata, peer discovery, native dequeue worker/completion adapter, runtime channel/deadline timers. No native frontend caller invokes the new bind entry point. Shared PHY/listener operation has not been qualified on the Dell. AirDrop is not functional in this checkpoint.

Default hardware activity is off. Do not enable latitude-awdl-native or select this bundle for normal boot. The selected USB 0.1.2 remains the next hardware qualification step; the user confirmed it has not yet been boot-tested. Collect the existing Continuity report after that reboot; no graphics stress tests are needed.

Sources are pinned to itlwm d695c971420cc8af0daa286dcd3297e8236c0a63 with MTL fixes 05b19dc81db6aedcf4058525981f7326f57c04e2. Native ABI work targets IO80211Family 1200.13.1 from macOS 15.8.1 build 24H32. Preserve the full local connected tree for rebuilding; this checkpoint stores the modified source and test inputs.
