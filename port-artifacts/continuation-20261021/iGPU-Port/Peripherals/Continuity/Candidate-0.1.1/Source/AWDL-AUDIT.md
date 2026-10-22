# Intel Continuity/AWDL integration — current evidence

Target wireless controller: Meteor Lake 8086:7e40; current working AirportItlwm
2.4.0 UUID 7D459253-2947-3ACB-BF9A-55F5E6CAD827. Base laobamac/itlwm revision
d695c971420cc8af0daa286dcd3297e8236c0a63. The exact CoreUltraGraphics firmware,
PNVM, internal-auxiliary-station and receive-aggregation patches were archived
from revision 05b19dc81db6aedcf4058525981f7326f57c04e2. They must be retained
when adding AWDL. A stock replacement would discard the MTL bring-up fixes.

The current EFI enables IntelBluetoothFirmware 2.5.1, IntelBTPatcher 2.5.1 and
BlueToolFixup 2.7.2. Presence/enabling of those files does not establish runtime
Bluetooth LE advertisements, Handoff, AirDrop discovery or completed transfers.
The earlier loaded-kext report establishes BluetoothFamily and firmware-driver
loading but contains no AWDL transfer test.

The exact Wi-Fi source has AWDL/P2P ioctl scaffolding: some handlers merely
return success, SYNC_ENABLED stores a Boolean, and frame-template accessors store
memory. getAWDL_PEER_TRAFFIC_REGISTRATION returns an unsupported/error value.
Those paths do not implement a synchronized radio data plane. Capability flags
or a virtual interface name cannot replace it. Existing README claims that
AirDrop/Continuity work are not corroborated by the reviewed captures.

Required implementation and validation:
1. Preserve the existing MTL firmware/PNVM/station/RX fixes, then make the
   Skywalk/AWDL virtual-interface lifecycle correct on Sequoia.
2. Implement and validate bounded peer/election/channel-sequence state plus
   vendor action-frame TX/RX, timestamp synchronization and availability-window
   scheduling. Connect them to the radio's channel/context APIs, not just ioctl
   getters/setters. Maintain the normal infrastructure association.
3. Add the AWDL IPv6 multicast/unicast data path, peer lifecycle and queueing,
   sleep/wake/reset handling and channel coexistence.
4. Verify Bluetooth firmware, LE advertising and Apple's sharingd integration
   separately from ordinary Bluetooth pairing/audio.
5. Confirm actual peer discovery and a user-selected AirDrop transfer with a
   second Apple device. Such a transfer is not performed automatically here.

collect-continuity.sh captures runtime identities, controller capabilities,
awdl0/llw0 state, relevant registry services and bounded recent sharing logs.
It makes no radio, pairing, capability or network changes. New AWDL radio code
is not enabled in the candidate until it exists and is compiled/validated.

References:
- https://github.com/laobamac/itlwm/tree/d695c971420cc8af0daa286dcd3297e8236c0a63
- https://github.com/VedCloud2/CoreUltraGraphics/tree/05b19dc81db6aedcf4058525981f7326f57c04e2
- https://github.com/OpenIntelWireless/IntelBluetoothFirmware

## Additional primary-source review, 21 October 2026

AirportItlwm-Tahoe at 9b7d5ffd72099cc75d358bb203e9608c599172d6 is archived
under research/airportitlwm-tahoe. Its author explicitly excludes AirDrop from
the validated release. Its AirportItlwm.cpp outputRaw80211Packet still frees the
packet and returns dropped; outputActionFrame and bpfOutput80211Radio free the
packet and return success. Virtual-interface publication does not replace our
missing Sequoia radio path. Do not copy its Tahoe binary over this Dell's
MTL-patched driver. Issue 1062 reports pending CHANNEL_IO AWDL packets unavailable
through tested legacy mbuf dequeue paths, and a panic when manipulating virtual
interface flow control. This is a lead for the native Skywalk packet contract,
not a validated AirDrop fix or authority to probe arbitrary vtable slots.
No new live AWDL TX path is claimed by this candidate.

- https://github.com/kgp-macPro/AirportItlwm-Tahoe/tree/9b7d5ffd72099cc75d358bb203e9608c599172d6
- https://github.com/OpenIntelWireless/itlwm/issues/1062#issuecomment-4930188874
