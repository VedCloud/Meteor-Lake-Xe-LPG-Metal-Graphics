# AWDL raw-action candidate 0.1.0

The complete Intel Sequoia kext compiled (74 source units), linked and passed
signature verification. It retains the exact Meteor Lake firmware/PNVM, AUX,
PHY/MAC/join and RX aggregation patch stack used by the working Wi-Fi lineage.
The candidate binary and build identity are separate from the active EFI driver.

The new path connects AirportItlwmV2's native bpfOutputPacket callback through
the controller command gate to ItlIwx::iwx_tx. It accepts only bounded,
validated radiotap-wrapped AWDL action frames on the current associated channel,
with the device's source MAC and AWDL BSSID. It refuses scan/flush/shutdown,
passive/DFS channels, a full queue and mismatched hardware/firmware. Existing
superclass behavior remains selected unless latitude-awdl-current-channel=1
is explicitly enabled. The package does not add this boot argument.

Host sanitizer tests exercise the actual new callback/HAL function bodies with
kernel/radio APIs mocked. They check packet/node ownership, submit failures,
allocation/copy errors, channel constraints and default behavior. These tests
do not establish hardware transmission, remote reception or successful AirDrop.

Important limits before a hardware trial:

- The native virtual-interface/raw-output callback must actually deliver a
  radiotap frame. The ordinary Ethernet BPF callback is a different path; do not
  send radiotap bytes through its normal Ethernet queue.
- A zero submit return is queue acceptance, not an RF completion or ACK.
- Native peer data, completion reporting, receive demultiplexing, off-channel
  scheduling and secondary MAC/station setup are not implemented here.
- No full AWDL/AirDrop working claim follows from compiling this candidate.

This candidate is staged, not installed or automatically armed. Keep the
working AirportItlwm.kext and the current EFI available. No firmware flash,
pairing, file transfer, network reconfiguration or packet injection is performed
by packaging or the read-only collector.
