# Current Tahoe port sources and packages

Published 30 October 2026. Complete graphics/audio 0.3.21, Tahoe Wi-Fi 0.1.2 and
matching source archives are indexed in [packages/20261030](../packages/20261030/).
The binary archives include the large shader compiler dependencies; no installable
runtime is silently missing libLLVM. This source checkout omits only the large
generated Network/include/FwBinary.cpp, whose exact bytes/hash are documented and
included in the complete source archive. Download that archive for complete source.

Graphics/plugin and translator match the 0.3.21 build; Runtime/ contains the
accelerator0.3.1, family guards, transaction/recovery sources and license notices.
Network contains the Tahoe 0.1.2 branch, SimpleHDA the audio sources, Pen the latest
1.0.6 experiment and Touchpad-experimental the 1.0.10 source plus its recorded
runtime failure. Neither the pen nib nor the touchpad is claimed fixed.
Original local builder paths are retained; this publication does not claim a new
portable clean-room build or a new hardware gate run. Earlier Sequoia packages
remain separate and retain their original versions/results.
