# XPS xe-brilho reference assessment

Source: https://github.com/JulioNectar/Hackintosh-XPS13Plus-IrisXe-Tahoe and user-pasted tools/xe-brilho.m. Readme marks native brightness keys unsupported and suggests xe-brilho. Its optional session CGEventTap consumes NX brightness events or configured ordinary key/modifier bindings and steps PWM via XeShim MMIO. It assumes keys already reach userspace; it does not decode Dell ACPI/WMI or revive a frozen PS2 keyboard. It requires Accessibility permission and is a logged-in-session workaround, not login-window/native firmware routing.

Useful alternatives: configurable shortcut fallback, explicit key-down handling, perceptual steps and persistence. Latitude already has native slider/PWM control: an adaptation should use that validated framebuffer brightness API, preserve native slider/OSD consistency and avoid a parallel raw-MMIO writer. Do not copy the XPS Raptor Lake register offsets or XeShim into the Meteor Lake setup based solely on this example. Input generation remains the unresolved part; first test the already-packaged WMI metadata correction and HID readguard candidate.

No new binaries/configs, MMIO writes, event taps or key monitoring enabled during this review. Candidate awaiting Dell test remains unchanged.
