# Security startup comparison — 30 October 2026

One-variable follow-up to the published full-engine baseline: set ASL_DISABLE=1
only in the guest com.apple.securityd launchd environment. Native program,
arguments, security checks and keychain contents remain unchanged. The source
image and initialized-memory input are preserved; a new owned COW HFS image and
exFAT fixture contain the variant. Filesystem and exact capture checks pass.

New EFI SHA256: 64281b166744dba729c2a289f21897622fca67feafe65e1cec1a32c5862aa9bf
New initialized-memory SHA256: 2a342d7015d59a1f94bad079eb98c2bfead2526adcff4a473bd6191c9d9bf00f

Reason: the baseline reached native WindowServer's server loop, but recoveryosd
waited in Security keychain initialization while securityd waited in
__asl_server_message. Apple's public Keychains.cpp describes a one-byte read
waiting for EOF from the system-keychain-check socket; the exact native stack is
consistent with that path. This is a test hypothesis, not a proven root cause.

This does not bypass a keychain check or fabricate completion. The initial test
setup stopped before QEMU because the new input-disk fixture was missing; it was
created and hash-verified before restarting admission. Runtime effect and a
visible installer are still unverified. Do not replace the published baseline
with this experiment. Paths in these preparation recipes are the original local
workspace paths; they are not generic host scripts to run unchanged.

References:
- https://github.com/apple-oss-distributions/Security/blob/main/OSX/libsecurity_keychain/lib/Keychains.cpp
- https://github.com/apple-oss-distributions/security_systemkeychain/blob/main/src/systemkeychain.cpp
