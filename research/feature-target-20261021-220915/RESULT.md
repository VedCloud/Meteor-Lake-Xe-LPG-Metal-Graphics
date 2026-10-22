# Dell candidate result — 21 October 2026, 22:09 London

Confirmed from the Dell capture:
- Loaded AirportItlwm 2.4.0 UUID AF812602-49E1-3342-AB04-7C32269891DE matches the new private candidate. The selected USB binary SHA256 is 383865bdaf1399a1b66482bf3da45361612ac4ebde4bea51c3e6a24a51a90ab6.
- en0 is connected on channel 48 / 5 GHz / 80 MHz, 802.11ac. The user confirms browsing works. This establishes ordinary Wi-Fi compatibility for this boot, not long-term stability or AWDL RF transmission.
- Both Intel native text fixtures PASS: opaque/transparent CoreText-to-CGImage composition and ordinary AppKit drawing with Arial and native San Francisco regular/bold variable fonts. Clipping, ownership and fallback tests pass. These are explicitly loaded test processes; existing Apple system apps have not been switched globally.
- The user confirms the boot chime sounded on this candidate boot.

Remaining AWDL evidence:
- ifconfig reports awdl0 and llw0 do not exist.
- IO80211AWDLPeerManager and IO80211P2PInterface queries return no services.
- No LatitudeAWDL raw-submit property was captured. The USB trial script did not arm the raw-radio gate. These logs do not exercise or prove the raw-action transmit path.
- Bluetooth is on; partial logs show BLE nearby discovery. This is separate from AWDL link operation.
- The historical continuity log search hit its 15-second limit (exit 143), retained partial output and reported a wall-clock adjustment. Do not infer an absence of all errors/events from this partial search.

No further full graphics or text-fixture rerun is needed for these same unchanged binaries. Remaining work is native AWDL interface lifecycle and real packet scheduling/TX/completion/RX, plus separate system-app text deployment and GPU-backed CoreAnimation integration.
