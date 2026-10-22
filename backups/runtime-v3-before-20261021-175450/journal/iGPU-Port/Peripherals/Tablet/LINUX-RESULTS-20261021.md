# Measured tablet and ELAN results — 21 October 2026

These are measurements from this Dell, not automatic macOS feature validation.

## Convertible signals
All five guided phases completed, with 50 samples. Intel HID switches reports
laptop=false, folded=true, left=true, right=true, laptop-return=false. The separate
Dell WMI switch stays false and is excluded from mode decisions. The session used
Linux 6.19.10 with intel_hid.enable_sw_tablet_mode=2. This does not establish that
macOS has enabled or receives the same events. Linux's INTC1077 driver interprets
ACPI notifications CC as tablet and CD as laptop.

The calibrated acceleration medians (m/s²) are:

| Physical pose | X | Y | Z |
|---|---:|---:|---:|
| Laptop | -0.272 | -6.961 | -6.978 |
| Folded landscape, held flat | -0.155 | 1.229 | -9.799 |
| Portrait left | 9.367 | 0.118 | -3.309 |
| Portrait right | -9.330 | -0.519 | -3.154 |
| Laptop return | 0.167 | -8.061 | -5.692 |

Flat samples hold the last orientation; zero quaternions are invalid. Inverted
orientation has not been captured. Physical labels are not framebuffer/touch
transform angles. The native C++ policy replays all 50 samples, uses a 500 ms
stable decision window, rejects malformed/stale/future samples, restores input
intent on laptop state, and fails open on mode loss. On-screen keyboard intent
requires tablet mode plus a fresh, editable focused field in an unlocked session.
This policy does not dispatch input or alter scanout by itself.

Four real sensor descriptors were decoded with bounded stack/size handling.
The accelerometer is sensor .0006, report ID 3, signed 32-bit XYZ at payload bit
offsets 208/240/272 (wire byte offsets 27/31/35 including the report-ID byte).
Its input report is 136 bytes including ID. Unit=0, exponent=-6; conversion must
follow HID sensor unit semantics rather than treating the integers as m/s².
The Linux IIO capture reports scale 0.000009806 m/s² per raw unit.

## ELAN gesture capture
The completed report has 4,007 frames across five phases, zero SYN_DROPPED, valid
contact counts, and up to four contacts. One/two-finger medians are about 6.92 ms;
three/four-finger medians 7.07/7.61 ms. Gaps during lift/swipe phases do not measure
WindowServer or prove a transport stall. The three-finger phase included a maximum
of four concurrent contacts; the report does not establish exactly how many
fingers the user intended to place on every frame.

ELAN 04f3:328e is on PCI 00:15.3 (8086:7e7b), ACPI I2C3.TPD0. Linux confirms
GPIO 387 -> physical pad 232, GPP_D_3, active-low level. Wacom uses I2C0 and a
separate GPIO 373. This confirms our existing mapping; changing it speculatively
would not fix the failed macOS bring-up. The staged LatitudeGPIO 0.1.1 adds raw
ownership/lock/enable state and exact set-interrupt refusal stage/result. It keeps
the existing MMIO programming and ownership guards. The failed touchpad trial is
not re-enabled in the normal profile.

## Candidate and remaining implementation
LatitudeTabletObserver 0.1.0 is a passive INTC1077/HIDD CC/CD event observer with a
separate IOMatchCategory. It does not evaluate ACPI, enable firmware notifications,
change EC/PCI registers, suppress input or rotate the panel. Its properties are
historical candidates, explicitly unqualified. Seeing no events can mean the
firmware interface is not enabled, not that the physical switch is absent.

A signed x86_64 Latitude Tablet Keyboard app provides a nonactivating manual touch
keyboard. Its automatic mode checks a future qualified live LatitudeTabletState
publisher, a <=1.5 s mode heartbeat and focused editable text role (no field value
reads). It stays hidden when that publisher is absent. Manual mode requires
Accessibility permission and can be tested independently. It is an initial basic
keyboard, with letters/numbers/space/Shift/backspace/Return/arrows, not an IME or
FileVault/login-screen keyboard. No startup job is installed.

Remaining: implement and validate macOS ISH/ISHTP transport, qualify live mode
signals, coordinate rotated framebuffer and Wacom transforms, release held keys/
contacts before per-device suppression, verify wake/recovery, and connect the
qualified publisher to the keyboard UI. Automatic rotation, tablet input blocking
and automatic keyboard popup are not enabled in this candidate. No rendering tests
need repeating for this observational update.

Primary references:
- https://github.com/torvalds/linux/blob/v6.19/drivers/platform/x86/intel/hid.c
- https://github.com/torvalds/linux/blob/v6.19/Documentation/hid/intel-ish-hid.rst
- https://github.com/torvalds/linux/blob/v6.19/include/linux/hid-sensor-ids.h
- https://support.apple.com/en-mk/guide/mac-help/mchlc74c1c9f/mac
