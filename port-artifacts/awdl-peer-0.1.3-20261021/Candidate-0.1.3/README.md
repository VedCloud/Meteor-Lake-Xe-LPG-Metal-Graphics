# AWDL peer transport engineering status — 0.1.3

Full AirDrop is NOT implemented or hardware validated by this candidate.
It is staged separately; it does not replace the USB's selected 0.1.2 driver.

## Changes implemented

- Kernel-safe Ethernet/IPv6 to AWDL data framing and reverse decoding, including
  the Apple LLC/OUI, AWDL header, peer addresses, multicast mapping and sequence
  handling. Input is bounded; fragmented, encrypted, DS and unsupported QoS
  frames are rejected rather than guessed. Peer admission is bounded to 16;
  data from unknown/expired peers and duplicate sequence numbers is rejected.
- A bounded mutual-channel window search over two validated 16-slot schedules.
  It requires a common synchronization clock domain and host-monotonic anchors,
  skips unavailable channels, reserves boundary guards and limits a dwell to
  32 TUs. Regulatory permission remains a separate HAL requirement.
- API83 session state and command encoding: await a matching successful START
  before admitting TX, enforce a deadline and home-channel guard, stop admission
  on timeout/failure, and wait for END plus all packet completions before reuse.
  Completion tokens reject duplicates and stale firmware epochs. Reset cleanup
  explicitly requires that the caller has already quiesced DMA.
- A real change to the Intel RX handler: validate the session notification's
  exact 16-byte layout, retain the original DMA synchronization, and separate
  infrastructure association events from peer events. Unrelated P2P or malformed
  notifications no longer clear the infrastructure time-event flag.
- Modern native AWDL declarations derived from UUID-matched 24H32 IO80211Family
  and its matching concrete Apple driver, with compile-only dispatch auditing.

## Verification performed

The complete Intel kext (74 translation units) compiled, linked and passed ad-hoc
signature verification. Strict ASan/UBSan fixtures exercised the actual Intel RX
case, session grant/deadline/credit/reset policy, channel-window intersections,
peer limits, sequence wrap/replay, multicast and all truncated frame lengths.
The data codec interoperates in BOTH directions with OWL's real data parser and
generator. No host radio was opened and no target kext was loaded on this Mac.

The native compile fixture checks all 563 dispatch slot names/offsets, the 63
typed AWDL slots, object size 0x120 and TxPacketRequest offsets 0x18/0x58. Private
placeholder declarations preserve dispatch layout; they are NOT callable method
implementations. The fixture returns Unsupported for AWDL requests and is never
linked into the candidate kext. This is an ABI audit, not a native AWDL driver.

## Runtime connections still missing

The scheduler is deliberately UNBOUND: no dedicated secondary MAC/link/PHY,
peer stations or TX queues have been created. No off-channel command is sent.
The frame codec is not connected to native awdl0 TX dequeue or RX delivery.
The native concrete interface, its 63 control handlers, synchronized schedule
publication, timer dispatch, firmware context transaction/rollback, native packet
ownership/completion and final AirDrop transfer remain required. In particular,
neither a software resource lease nor a successful command ACK proves firmware
ownership, radio airtime, peer reception or a successful file transfer.

The 0.1.2 current-channel action bridge remains the separate hardware transport
qualification path. The latest Dell report still identifies 0.1.0; no Dell report
has yet demonstrated 0.1.2 action TX/RX. This package does not enable its TX gate.

## Exact native evidence

Target: macOS 15.8.1 build 24H32, IO80211Family 1200.13.1,
x86_64 UUID 13D3592A-A6BE-35E7-984C-0ACD90A76356, matching the Dell report.
The matching KDK was extracted for offline analysis and never installed.
Its read-only disk image mount has been ejected.

The default native createVirtualInterface accepts role 7 only; its default
IO80211VirtualInterface allocation is abstract. requestPacketTx is empty.
Consequently superclass delegation cannot provide an operational AWDL interface.
Modern IO80211VirtualInterface inherits IO80211SkywalkInterface, not the older
Ethernet interface layout. The modern object is 0x120 bytes, with 500 slots;
IO80211AWDLProtocol adds 63 slots (563 total). These observations explain why
an older Ventura object layout or simply advertising AWDL flags is insufficient.

Firmware API83 advertises SESSION_PROTECTION command v1 / notification v2.
Linux v6.5 uses that command for P2P ROC when the firmware capability is present;
the newer ROC command must not be assumed compatible with this firmware.

Sources:
- [Linux v6.5 time-event implementation](https://github.com/torvalds/linux/blob/v6.5/drivers/net/wireless/intel/iwlwifi/mvm/time-event.c)
- [Linux v6.5 firmware time-event API](https://github.com/torvalds/linux/blob/v6.5/drivers/net/wireless/intel/iwlwifi/fw/api/time-event.h)
- [Pinned OWL implementation](https://github.com/seemoo-lab/owl/tree/0ee3263f944ea721c355f826aa90d636d012a512)
- [Exact-build KDK mirror](https://github.com/dortania/KdkSupportPkg/releases/tag/24H32)

Graphics, keyboard/Copilot mapping, working Wi-Fi selection and OpenCore settings
are preserved. No GPU stress suite is requested by this package.
