# AWDL current-channel radio bridge 0.1.2

Real driver connections implemented:
- Root-only IOKit user client: hardware/status query, bounded action-frame receive
  dequeue and gated action submission to ItlIwx::iwx_tx. Ordinary Apple Wi-Fi
  user-client types continue through the native superclass.
- Copy matching AWDL vendor action frames from the existing Intel RX path into
  a 16-frame FIFO, with payload validation, overflow accounting and reset cleanup.
  No firmware RX filter changes, channel changes or ordinary packet consumption.
- The userspace tool links the real OWL action encoder/receiver: PSF/MIF creation,
  bounded peer handling and current-channel state. TX uses the existing firmware
  status/cookie/reclaim/reset ledger, rather than treating enqueue as completion.
- Stop closes the API before draining the controller gate and detaching the HAL.

Full Intel kernel compilation/link/sign and Intel userspace compile/sign pass.
Strict ASan/UBSan tests cover the actual user-client body and protocol boundary.
OWL's unaligned wire accesses and overflow-prone offset checks were corrected in
an archived private overlay. Upstream checkout and license notices are retained.

Hardware transmission and passive RX delivery are not validated. The firmware
may still filter frames from the AWDL BSSID; this build does not pretend otherwise.
The userspace poller is a transport qualification tool, not a native Finder service.
AirDrop remains unfinished: off-channel scheduling, native Skywalk peer data,
IPv6/mDNS and a completed transfer with another Apple device remain required.
No awdl0/llw0 interface is manufactured or capability falsely advertised.

Usage after rebooting with this candidate:
sudo ./latitude-awdl-radio-x86_64 status
sudo ./latitude-awdl-radio-x86_64 listen 30

These two commands do not transmit. A one-frame send-psf/send-mif command requires
both --transmit-current-channel and the separate latitude-awdl-current-channel=1
boot gate. The package/installer do not enable this gate or automatically send.
RF results cannot be inferred from successful compilation or protocol unit tests.

trial-awdl.sh apply selects this USB Wi-Fi bundle; restore selects the original.
A copy of the Dell-tested 0.1.0 bundle is also retained. Graphics, keyboard,
OpenCore configuration and the validated 26,000-frame GPU suite are unchanged.
