# AWDL candidate 0.1.1 — native lifecycle and transmit accounting

Full 74-unit Intel Sequoia compilation, linking and ad-hoc signature verified.
Preserves the exact 0.1.0 Meteor Lake firmware/PNVM and ordinary Wi-Fi fixes.

Adds per-frame cookies before the ring kick; firmware response status is counted
once, independently of queue acceptance and buffer reclamation. Reset aborts
release the raw packet's node reference. These counters do not prove remote
reception; a broadcast success is not a peer acknowledgement.

Virtual-interface overrides delegate to Sequoia's native factories, recording
create/attach/enable/disable/detach outcomes. No old Ventura object is allocated,
no fictitious AWDL capability is advertised and no link is forced active.

AirDrop is unfinished: peer TX/RX, native CHANNEL_IO queues, off-channel timing
and target firmware qualification remain. No automatic RF transmission occurs;
latitude-awdl-current-channel=1 remains absent from the USB configuration.
This candidate is staged, not selected in EFI. The original and Dell-tested
0.1.0 bundle are accepted predecessors; restore returns the original Wi-Fi.
No graphics test suite is requested.

Source/0.1.0-to-0.1.1.patch applies after the archived 0.1.0 patch stack.
