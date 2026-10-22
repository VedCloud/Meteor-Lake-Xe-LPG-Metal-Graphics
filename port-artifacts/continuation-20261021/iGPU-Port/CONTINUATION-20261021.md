# Continuation: AWDL, native text formats and sleep

AWDL 0.1.1 compiles/links/signs and is staged separately. AirDrop is not functional.
The active EFI remains the Dell-tested 0.1.0 bundle.

Text adds native BGRA channel-order handling and all RGBA/BGRA transparent-image
combinations. Host ASan/UBSan and release tests pass; Intel artifacts compile/sign.
This is process-local rendering support. System-wide Apple-app and GPU-backed
CoreAnimation integration remain unfinished. No global injection is installed.

Sleep symptom: stays awake. The active DSDT has Name(SS3,Zero) and guarded _S3;
no forced _S3 patch is applied. The GPU callback state-error fix compiles as an
Intel object and passes failure/retry tests, but is not installed. No sleep fix
is claimed. The new read-only before/after collector gathers the live evidence.
The desktop collector also now records sleep capabilities and scheduled wakes.

On the Dell, before trying Apple menu Sleep:
/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Power/collect-sleep.sh before
Then, if it remains awake or after waking:
/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Power/collect-sleep.sh after

Native integration evidence, without RF transmission or app injection:
/bin/bash /Volumes/SEQOUIA/iGPU-Port/Peripherals/Continuity/collect-native-integration.sh
