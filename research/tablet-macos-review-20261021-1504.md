# macOS laptop/tablet review — 21 October 2026, 15:04

Reviewed and archived macos-20261021-150424-laptop and macos-20261021-150445-tablet.
Both native inventories completed successfully, helper 0.2.0. Both show one built-in
1920x1080 display at rotation 0, no sensor IOHID services, no LatitudeTabletObserver
service, and unknown tablet state. The observer ioreg text is empty in both; an
exit=0 is a successful query, not proof of a matched driver.

Full IOACPIPlane plists confirm ISHD@120000 exists in both, but no HIDD node.
Therefore the passive observer has no enumerated ACPI provider to bind to in
these reports. The USB config currently enables LatitudeTabletObserver, but these
snapshots do not establish whether its binary loaded on the tested boot.

Saved firmware source contains HIDD/INTC1077 _STA returning zero when OIDE()<1.
OIDE invokes OSID and returns ACSE. OSID starts ACSE at zero and enables it for
Windows 2012/2013 _OSI responses. This is a concrete firmware visibility gate
consistent with the macOS absence, not a measurement of the current ACSE value.
No ACPI methods were evaluated and no firmware state was changed by this review.
A targeted HIDD visibility solution should be assessed before enabling broader
Windows identity spoofing, which could also change I2C and other firmware paths.

NMXVGAFB registry output is populated in both, with active NMXIntelVGAFB,
IOFBTransform=0, and LatitudePanelRefreshMilliHz=63739. Presence of the framebuffer
does not by itself prove GPU acceleration, but neither report indicates a new
portrait configuration or a successful sensor transport. No driver/config changes
were made during this review. Existing graphics and confirmed keyboard mapping
remain untouched.
