> **Current package publication — 30 October 2026:** [Complete Tahoe 0.3.21 graphics/audio, matching Wi-Fi and source packages, and the macOS27 full-engine diagnostic](packages/) are now published. Historical entries below retain their original dates and test scopes. macOS27 has reached native WindowServer startup; its graphical installer is not verified.

# Dell Latitude 5350 — Meteor Lake Xe-LPG on macOS: project roadmap

The dated record, **8–30 October 2026**, with the historical timeline oldest first. Every entry states what happened, what it
proved, and where the evidence lives.

The live authoritative status is `UNIFIED-CANDIDATE-WORK.md`; the invariants that must survive every
rebuild are in `AGENTS.md`; the evidence trail is `research/` and `port-artifacts/`.

## Where we are now (30 October 2026)

* **The Intel graphics port now covers Sequoia and installed Tahoe 26.7.1.** The retained Tahoe
  graphics checkpoint is 0.3.21, with the custom Dell icon and requested graphics label. The original
  **Sequoia** application gate remains 26,000 frames / 22 cases / 80,020 command pools. It has not been
  rerun as a new Tahoe full-gate result; 5.0 ms readback is one stage, not total frame time.
* **Working hardware features:** Wi-Fi, Bluetooth, ALC295 audio, panel brightness, Night Shift and
  touchscreen on the retained profiles. Tahoe audio uses SimpleHDA and requires `keepsyms=1`, which
  does not enable verbose boot. Copilot → Command, boot chime and Windows/Fedora booting are confirmed.
* **Open hardware work:** touchpad lag/keyboard contention, remaining graphics latency/glyph issues,
  brightness-key-to-slider/HUD integration, sleep/wake, tablet/rotation, system-wide FreeType and
  full AWDL/AirDrop. Packaged experiments are not promoted as passing hardware results.
* **macOS 27 has advanced beyond preflight to native recovery services and WindowServer's server loop.**
  The current x86 UEFI engine exposes 16 GiB logical guest memory. Its latest display capture is still
  black; a graphical installer and physical Dell installation are not verified.
* **The actual packages are published.** [The 30 October release](packages/) includes complete Tahoe
  graphics/audio, the matching Wi-Fi binary, corresponding source and the current macOS27 diagnostic
  engine. Older Sequoia packages and frozen test reports remain separate.

<a id="macos27"></a>
## macOS 27 — dedicated standalone-runtime roadmap

**Updated 30 October 2026.** Target: the supplied ARM64 build `26A428`, executed through an
ARM-to-x86 translation engine beneath the native kernel. The target remains the **Dell Latitude 5350
with 16 GB RAM**. CPU/firmware qualification currently runs in an owned x86 OVMF test; it is not a
physical Dell boot result or a native ARM instruction-speed benchmark.

### Verified milestones

| Stage | Evidence and boundary | Result |
|---|---|---|
| Initial 22 October preflight | Hash-pinned inputs, bounded scalar/privileged probes and MH_EXECUTE bounds tests; the original 0.0.3 source is preserved | Complete historical milestone |
| Full firmware engine | Compiled x86 UEFI engine with MMU/COW, exception, PAC, timer/interrupt and selected Apple-device support | Executes the native kernel; no longer limited to the original scalar preflight |
| Kernel and recovery root | Native Darwin27 banner, pmap startup and HFS recovery-root bring-up | Reached |
| Recovery userland | Native launchd, recoveryosd, OpenDirectory, LaunchServices and CoreServices observed | Reached |
| WindowServer | Latest run has `ws_main_thread`, CoreAnimation render-server/display and HID threads. Saved return addresses resolve to `_CGXRunOneServicesPass`, `_server_loop` and `_SLXServer` | Native server loop reached; not a visible installer |
| Memory and boot inputs | 16 GiB logical guest RAM with bounded sparse backing, checked initialized-memory input and firmware entropy | Implemented; not a claim that the entire 16 GiB is physically resident |
| Firmware/CPU correctness | 4,096 QARMA comparisons, 72 monitor/PAC checks, 768 SPRR checks and exact-signature observer fixtures | Component tests passed |
| Current performance candidate | Word-wise memory operations, observer offset prefilter and unchanged-frame GOP suppression; real firmware checks and modified-capture rejection pass | Integrated boot under observation; no completed boot-speed claim |
| Actual display | Latest 1280×800 OVMF display capture contains zero nonblack pixels; the native framebuffer is registered/started but no client is open | Graphical installer not verified |

### Current candidate and reproducibility

The latest WindowServer-qualified baseline's EFI SHA256 is
`90ae91ef8b15b852f282707019f7df068576d2bfac97e499fde571fe96e1ad0f`.
Its exact source, pinned upstream overlay and original build/admission evidence are published in
[`macos27-standalone/full-engine/`](macos27-standalone/full-engine/). Download the matching diagnostic
archive and checksums from [the package index](packages/20261030/).

The baseline boot began 29 October and was observed into 30 October. A separate modified-input run
passed its rejection check before that boot. Its final black capture and native service states were
saved before a controlled stop for the next one-variable comparison. Persistent run ownership/state/exit records now distinguish a
host process exit from a guest panic; the earlier unexplained host exit remains unexplained.

Apple installer/recovery images, SDK/KDKs, trust caches and initialized-memory captures are not in the
public package. The diagnostic depends on those checked external inputs; its source snapshot retains
local workspace build paths. It is not a one-command portable installer build.

**Follow-up under test:** the [security-startup-v12 experiment](macos27-standalone/experiments/security-startup-v12/)
changes only securityd's guest `ASL_DISABLE=1` environment setting. Compilation, filesystem verification
and exact input checks pass. All 35 firmware/CPU markers and the modified-capture rejection test
passed; the valid-input boot is running. The logging change's runtime effect remains unverified. Its EFI hash is
`64281b166744dba729c2a289f21897622fca67feafe65e1cec1a32c5862aa9bf`.
The first setup attempt stopped before QEMU because its private input disk was missing; that fixture
was then created and checked before admission was restarted. The published baseline is unchanged.

### Current investigation

Two observations guide the next work; neither is yet a proven root cause:

1. **Display adoption:** WindowServer runs its native loop and CoreAnimation/HID threads, but the
   framebuffer has no open client and no physical display is registered. CoreDisplay/renderer
   enumeration and the guest's expected display transport need to be traced. A console framebuffer
   alone is not an interactive display pipeline.
2. **Recovery startup:** the latest selected recoveryosd stack is waiting in native `_read` beneath a
   Security keychain initialization/`SecKeychainLogin` call chain. Identify the descriptor and waiting
   service from bounded metadata before diagnosing entropy, IPC or a file-read problem. No keychain
   contents, credentials or PAC keys need to be read or replaced for that diagnosis.

The existing no-Metal compatibility option follows a genuine timeout's native cleanup path. It does
not implement a compositor or make the engine accelerated. Likewise, an OpenGL software-renderer
module in the shared cache does not prove WindowServer selected or used it.

### Next implementation gates

| Order | Work | Acceptance evidence |
|---|---|---|
| 1 | Identify the display/renderer and recovery-startup dependencies from the current live run | Exact-build symbol/signature checks, selected thread states and correlated service/driver events |
| 2 | Implement the missing verified transport/service behavior in an isolated candidate | Positive and negative component tests; no fabricated completion, disabled error or guessed register value |
| 3 | Bring up actual graphical recovery/installer presentation | Inspect a real capture containing recognizable native recovery/installer UI; a colored test pattern is insufficient |
| 4 | Connect installer input | Pointer, click, keyboard and focus work in the actual UI |
| 5 | Qualify target storage | Correct disk/partition discovery and bounded I/O; a disposable target passes before any real Dell disk is used |
| 6 | Qualify the physical Dell path | Boot from a separate diagnostic entry with the working Tahoe EFI preserved; actual installer output on the panel |
| 7 | Optimize after correctness | Measure full boot/frame/presentation latency and memory use; do not compare scheduled instruction slots with frames or retired instructions |

**Installer milestone:** visible native installer UI **and usable input**, with its target storage
identified and verified. It remains open. The Tahoe desktop screenshot and Sequoia's application gate
are separate achievements and cannot satisfy it.

### Keep the working Intel port intact

The current Tahoe quiet multiboot EFI is preserved. The separate macOS27 loader and checked inputs were
staged alongside it without replacing the main loader. Historical passing checkpoints, Copilot mapping,
SimpleHDA's `keepsyms=1` requirement and normal accelerated-boot policy remain unchanged. No installer
or test script automatically reboots the Dell.

## How to read this document

* **What worked** is stated with the measurement that established it. A build, a compile or a
  `desktop exit=0` is not a hardware result.
* **Negative results stay negative.** A defect that was found, fixed and then falsified on hardware is
  recorded as a correctness fix *and* as "not the cause" — both halves.
* **Counts are exact and scoped.** 26,000 frames is that matrix, not a claim about all workloads.
  80,020 pools is that run's object accounting, not a guarantee about every cache.
* Plugin submission IDs and i915 request sequence numbers are different namespaces and are never equated
  without correlation.

---

# Timeline

## 8 October — Phase 0: the kext would not inject

OpenCore was silently discarding the kext:

```
KextFindKmodAddress      → Kmod = 0
InternalPrelinkKext      → EFI_UNSUPPORTED          (Link.c:1629)
PrelinkedInjectKext      → EFI_INVALID_PARAMETER    (reported quietly)
```

The binary had **no `_kmod_info` symbol**, so OpenCore had nothing to link the kext against.

* Xcode 15.4's SDK contains no `kmod_info*` and no `CreateKModInfo*` (CI `37789629143`, `37789952837`).
* MacKernelSDK does not define it either (pinned `3f750085caa17ec3a7880f11c11bf4f48cd6a164`); it was
  therefore *not* vendored.
* Eight different link-flag arrangements all failed. The symbol was simply absent.
* A **dummy `_kmod_info` was rejected on principle** — satisfying a guard with a fake structure would have
  produced a kext that linked and then misbehaved.

The breakthrough was reading `libkern/kmod/README` instead of guessing: the object emitted by XNU's
`CreateKModInfo.perl` must be compiled and placed **between `-lkmodc++` and `-lkmod`**. That became
`kmod_info.c` using `KMOD_EXPLICIT_DECL`, with a `check-kmod-info` build guard so the failure could never
silently return.

```
OC: Prelinked injection CoreUltraGraphics.kext (…) - Success
OCAK: Local relocs 313
```

**Lesson:** a build that produces a `.kext` is not a build that can be injected. The guard now fails the
build, not the boot.

## 8 October — Phase 0b: macOS froze at EXITBS:START

```
AAPL: #[EB.LD.OFS|OPEN!] Err(0xE) <"usr\standalone\OS.dmg.root_hash">
AAPL: #[EB.RH.LRH|P!] Err(0xE) <- EB.LD.LF
AAPL: #[EB.BST.FBS|!] Err(0xE) <- EB.RH.LRH
AAPL: #[EB|LOG:EXITBS:START]
```

`Err(0xE)` is `STATUS_NOT_FOUND`. Apple's `boot.efi` wanted a root-hash file that was **not inside the
recovery image at all** — confirmed by listing `BaseSystem.dmg` directly: zero matches for `root_hash`.

Three controlled experiments proved this was not our configuration:

* **ACPI** — bisected from 8 SSDTs to 2, both rename patches off, kext off. Byte-identical failure.
* **SIP** — `csr-active-config = 0x7F000000`; the log confirms `EB.CSR.S 0x7F` and `EB|CSR:OUT 0x6F`.
  Identical failure.
* **Quirks** — verified against the MATs guidance (`OCABC: MAT support is 1`) and against a known-good
  Latitude 7400 EFI. Ours already matched.

`DmgLoading = Any` would not help either: it *relaxes* verification, and the file genuinely did not exist.

**Lesson:** "invariant across every config change" is evidence about the *image*, not the config. Chasing
`SetupVirtualMap`, `DevirtualiseMmio` and `DmgLoading` was time spent on the wrong layer.

## 18 October — Phase 1: the pivot to NullMoth

WhateverGreen was the obvious candidate and was ruled out from its own documentation: the last
documented Intel family is Ice Lake, and it supports neither Alder Lake nor Meteor Lake.

Rather than write a graphics driver from scratch, the project retargeted **NullMoth's NVIDIA driver**,
keeping its Metal plugin, shader translator and userspace architecture and replacing every NVIDIA element
with an Intel one. That decision is why the project reached real GPU submissions at all.

The first target attempt, `MyIntelGPU`, failed **authentication** on the laptop — a signature problem, not
a Meteor Lake problem. Rebuilding as an i915-derived kext under a custom SIP mask turned that into an
attach.

## 18 October — Phase 2: assembling a stack that had never run

* **438** i915, DRM/TTM, LinuxKPI, compatibility and IOKit sources compiled into `NMXGPU`, with AMD and
  Xe drivers deliberately excluded and **no unresolved driver-internal C symbols**.
* **1,045** ANV build steps for the Intel Vulkan backend, plus CLC tools built separately for the Apple
  Silicon host.
* Firmware matched to the laptop's own Linux report rather than guessed: GuC **70.53.0**, DMC **2.23**,
  HuC **8.5.4**, GSC **102.1.15.1926**.
* A one-shot NVRAM opt-in so a failed attach cannot silently re-arm on every boot.

The PCI probe then succeeded on the real `8086:7D45`, and `NMXAccel` began declining startup unless
`NMXGPU` had actually claimed the device — a guard that later proved its worth.

## 18 October — Phase 3: first submissions, first hang (0.2.4 → 0.2.9)

**0.2.4** reached the panel and copied an IOSurface into it — but all 64 diagnostic samples were zero, and
host allocation imports returned `-1000072003` (`VK_ERROR_INVALID_EXTERNAL_HANDLE`), falling back to
private images. Fence waits then returned `VK_TIMEOUT` after 5000 ms. WindowServer died with
`EXC_BAD_ACCESS / KERN_PROTECTION_FAILURE` **inside the main thread's stack guard**, after 512 frames
containing 255 `RenderEncoder` and 254 `SetScissor` calls. That was **stack exhaustion through recursive
SkyLight rendering after command-buffer failure**, not a standalone logging bug. GPU HANG `12:1:00dfffff`
followed at 19:13:06.637.

**0.2.5** narrowed it. The first four submissions returned `VK_SUCCESS`; **submission #5**
(`nvmtl_vk_image_read`) and the concurrent upload **#6** both timed out. `INTEL_DEBUG=stall` was confirmed
*enabled* on the failing boot — so it is a recorded **negative result**, not a missed setting.
Terminal-error command buffers bounded the recursion (retries spaced ~16 ms apart instead of tens of
thousands of calls), but WindowServer still aborted with `pipeline: Vulkan is not up in this process`.

The kernel's captured `reset_count=0`, `EIR/PGTBL_ER/fault_data=0` and zeroed ring fields looked
reassuring. They were not evidence of anything: the capture was incomplete.

**0.2.6 / 0.2.7** added a readback correctness patch and forced-route enumeration; 0.2.7 recovered
`VK_ERROR_DEVICE_LOST` after 13,072 ms and `GPU HANG 12:1:00b79797`.

**0.2.8 was invalid as a test.** The release constructor stripped **every** `NVMTL_*` environment variable
before backend initialisation, so fresh-pool, diagnostics, readback-trace and staging settings were all
inert. Every earlier release-environment A/B conclusion had to be withdrawn. **0.2.9** preserved six exact
controls only for the named test process and added an in-driver **effective-policy query**; the workload
now refuses to start on any mismatch.

**Lesson:** "the first four submissions succeed" is not progress if the fifth is where it dies. The first
*uncompleted* submission is the measurement.

## 18 October — Phase 4: the GuC lost-wake race (0.3.0)

**0.3.0** introduced fresh command pools. It passed four 1000-frame cases cleanly, then:

```
23:54:50.264  GuC TLB invalidation response timeout, seqno 1
              (after the padded process finished, before the next connected)
fifth case     hang on upload submission #1285, zero-based frame 428
              fence returned -4 after 10,798 ms
kernel         rcs0 request 12:2574 not yet started, GPU HANG 12:1:00b79797
pools          1286 created / 1285 destroyed / 1 quarantined
```

The hang was real, and fresh pools were **not** the fix. But the TLB timeout pointed at the waiter, and
that produced the most valuable software find of the project.

Linux's i915 invalidation waiter sets `WQ_FLAG_WOKEN` and sleeps. Linux's woken callback *also* persists
`TASK_WAKING`. The FreeBSD LinuxKPI callback in this port marked the flag and signalled the channel but
**did not persist `TASK_WAKING`**. `schedule_timeout` checks `TASK_WAKING` under that channel lock before
registering sleep — so a reply arriving between the outer flag check and registration could signal an
**empty channel**, leaving the waiter to sleep to expiry with the flag already set.

The fix sets `TASK_WAKING` under the same lock, following the port's existing `wake_up_task` protocol. The
real old and patched callbacks, the real `schedule_timeout` and the real i915 `must_wait_woken` were
extracted into a deterministic fixture: the old version **reproduces the false expiry**, the patched one
preserves positive remaining time, across **100,000 scenarios** under ASan/UBSan.

It is a verified software fix. It is **not** a claim to have cured the Dell hang, and the report says so.

## 19 October — Phase 5: the gate that finally meant something (0.2.8 → 0.3.1)

Two more failures had to be fixed before a pass was trustworthy.

**The tests were measuring nothing (0.2.8).** `nvmtl_conf_knobs()`'s release constructor stripped every
`NVMTL_*` variable before backend initialisation (see Phase 3). The shell's record of *requested* settings
was not evidence of *effective* settings.

**The collectors were selecting the wrong evidence (0.3.0).** One report required `results.txt`, so an
early preflight exit silently selected an older report whose frame-101 device loss belonged to a different
candidate. Another had hard-coded timestamps and attributed an unrelated historical hang. Selection was
rewritten to take the newest report even without results, record the boot time, and preserve preflight
metadata.

Only then did the gate mean what it claimed:

```
22 cases           11 IOKit + 11 direct
26,000 frames      1000 iterations per case
80,020 pools       created = destroyed = completed + abandoned
0                  live / pending / quarantined
```

**0.3.1** carried the lost-wake fix in kernel 1.0.8 and passed the full gate — the standing baseline that
makes "Metal and Vulkan work" a measured statement rather than an inference.

## 19 October — Phase 6: WindowServer, one variable at a time (0.3.2 → 0.3.5)

| Candidate | Struggle | What it proved |
|---|---|---|
| 0.3.2 | Black screen, **no retained evidence** — only a live-log header and an empty registry | An unbounded `pthread_mutex_lock` preceded the GPU wait budget |
| 0.3.3 | 8 submissions, 3 readbacks, then readback #4 timed out; `GPU HANG 12:1:00dfffff` | Bounded `trylock` + deadline; the hang is not host-lock contention |
| 0.3.4 | First 7 submissions, then **submission #8** (third readback) timed out with dedicated fresh staging | Staging reuse is **not required** to trigger the hang |
| 0.3.5 | Driver installed but never loaded | The AuxKC referenced a deleted temporary bundle |

Three diagnostic defects had to be repaired before the failures could even be read:

* **Sandboxed journal writes were denied** (`errno 1`) even with correct ownership. Stage records moved to
  the existing syslog channel; no sandbox or SIP change was needed.
* **A per-format eight-message throttle hid submissions past #8**, despite the caller's finite budget.
  Startup submissions now bypass it.
* **`sample` returned 0 while reporting `pid_for_task` failure and process 0**, producing no stack. The
  loader now resolves an explicit positive PID, refuses 0/1, and records missing output as a failure.

And the kernel capture itself was lying by omission:

```
intel_engine_coredump_alloc  → legacy register capture only when GUC_CAPTURE is absent
GuC captures                 → attach a parsed register-list node instead
previous summary             → printed legacy engine->head/tail/start/acthd, all zeros
```

Those zeros did **not** prove an idle ring, physical address 0, or the absence of a fault. Kernel 1.0.9
records the owned request's fence context/sequence/flags/error, readiness and completion states, pending
dependencies, GuC ID and scheduling snapshot, LRC address and ring/batch GPU virtual addresses, plus a
bounded parsed-GuC printer (3 lists, 64 registers per list/engine). It changes **diagnostics only** — not
scheduling, reset policy or timeouts.

The 13:36 boot then added one more lesson, and it was self-inflicted: the installer built a preflight AuxKC
from a temporary source path, installed that collection, then deleted the source, so the kernel manager
reported a missing bundle and WindowServer waited in `GPUWranglerWaitForPublish` for a GPU that was never
published. Local mocked cache tests had checked file contents and rollback but **not recorded source
paths**. The installer now builds the final AuxKC from the permanent installed path, inspects prelink
metadata for that path, refuses the temporary one, and verifies the candidate UUID.

**Lesson:** an installer that can produce a silent "driver installed but not loaded" state is worse than one
that fails loudly. Rollback tests must assert *where* files came from, not just that they exist.

## 19 October — Phase 7: a ring capture and a falsified defect (1.0.10, 1.0.11)

Kernel 1.0.10 is a **diagnostic-only** build: it saves the bounded words around each active render-ring
request at submission and re-reads them at hang. The 17:14 boot reproduced the familiar stall —
submission #8 / readback #3 — and the capture finally showed *content*, not just a pointer: the words below
request 20's head had become `0xff000000`, and the GuC `IPEHR` matched.

The capture window is `[head − 16, wa_tail + 8)`, so it also contains the request's *own* ending:

* At request 18's submission, `0xc98–0xca4` hold `fedca540 / 0 / 02800000 / 0` — request 18's own
  switch-out `MI_STORE_DATA_IMM` and its `MI_ARB_CHECK` / `MI_NOOP` workaround tail.
* At request 20's submission those same offsets read `ff000000 ×4`.
* At request 16's submission `0x9c8–0x9d4` already read `ff000000`, while `0xb30–0xb3c` was still intact at
  request 18. The overwrite is therefore not uniformly applied to every retired request.

GuC independently shows the GPU fetching that word: `IPEHR=ff000000`, `INSTDONE=ffdfffff`,
`RING_HEAD=ACTHD=0xca4`, `RING_TAIL=0xe08`.

A source audit then found a **genuine defect**, independent of the hang. The port's mapping-revocation shim
`lkpi_unmap_mapping_range()` is deliberately empty, and it is safe only because mappings are supposed to be
pinned for their lifetime. `drm_gem_ttm_mmap()` installs that lifetime pin (`nmx_ttm_pin`), but the i915
`vm_ops_cpu` path (`WB`/`WC`/`UC`) never did. On Linux, `put_pages` revokes the user PTE before the backing
is freed; the port cannot revoke, so an idle *mapped* object's pages could be reclaimed underneath a live
user mapping.

Kernel 1.0.11 is the fix candidate: pin i915 CPU user mappings for the lifetime of the mapping and release
the pin after the IOKit mapping is gone; refuse the legacy `I915_MMAP_TYPE_GTT` alias the port cannot
revoke; report `i915_gem_mmap_gtt_version() = 4`. It links **438 objects** with the same undefined-import
set as 1.0.8, UUID `B972B890-…`, SHA-256 `496e108b…`, and passes a 20,000-lifetime ASan/UBSan fixture plus
the installer/rollback fixtures.

Then the 17:32 boot with 1.0.11 failed **the same way**: same submission, same fence timeout, same
`0xff000000`, same `IPEHR=ff000000`. The lifetime-pin omission is a real port bug and stays in the tree
as a correctness fix — but it is **not the writer** of this overwrite.

**Lesson:** a real defect is not the same thing as *the* cause. The pin is well-motivated and provable in
source; the capture proved it was not sufficient. Keep both facts.

## 19 October — Phase 8: from "which submission" to "which writer" (1.0.12)

Apple's `IOBufferMemoryDescriptor::initWithPhysicalMask` decides host contiguity like this:

```c
mapped = (NULL != IOMapper::gSystem);          // only if kIOMemoryMapperNone is absent
contig = (0 != (options & kIOMemoryHostPhysicallyContiguous));
if (!contig && (0 != (options & kIOMemoryPhysicallyContiguous))) {
        contig |= (!mapped);
        contig |= (0 != (kIOMemoryMapperNone & options));
}
```

So `kIOMemoryPhysicallyContiguous` by itself requests **host**-page contiguity only when no system mapper
exists (or when `kIOMemoryMapperNone` is also set). The port's `nmxVramAlloc()` requested only
`kIOMemoryPhysicallyContiguous` and then exported the surface with
`getPhysicalSegment(0, nullptr, kIOMemoryMapperNone)` — taking the **first segment's base** and ignoring
the segment length it had just been handed. `NMXAccel` converts that base plus the surface size into an
`IODeviceMemory` physical range, so with scattered pages the exported range extends into memory the
allocation does not own.

Kernel 1.0.12 and framebuffer 0.2.4 correct the assumption: they add
`kIOMemoryHostPhysicallyContiguous | kIOMemoryMapperNone`, reject an export unless the first segment
covers the whole request, and release the slot and budget on rejection. The host regression uses Apple's
actual contiguity decision (copied from the Sequoia source, not reimplemented) plus the real old and new
allocator functions under ASan/UBSan: the deliberately fragmented allocator makes the old code export a
4 KiB segment for an 8 KiB request and the new code reject it; 2,000 allocate/free cycles leave no live
descriptor and no leaked budget.

**The caveat, recorded honestly:** the same boot log says `AppleVTD is not yet installed as gSystem`, and
the i915 first-error capture reports `iommu=0`, so on *this* machine the old flag may already have produced
contiguous pages. The correction is still right — the export should never have trusted a base address
without its length — but it is a **candidate**, not a demonstrated fix.

**Lesson:** a mechanism that can explain the value is not the same as evidence that the mechanism ran.
Record the precondition you have not measured next to the fix you have.

## 19 October — Phase 9: a boot panic (1.0.13)

A different hypothesis: i915's MTL **stolen-memory region** overlaps the firmware panel buffer, because the
port skips display discovery for `8086:7D45` and never performs Linux's initial-plane framebuffer handoff,
so a ring could be allocated from memory the panel still scans out. Kernel 1.0.13 withholds both stolen
regions on this device ID and logs the ring's `stolen=` state.

The installer completed at 18:13; the boot **panicked at 18:17:31**, 8.04 s in, inside
`guc_retire_inflight_request_prio`, locking a zeroed `intel_context.guc_state.lock`. The desktop was never
reached.

The backtrace proves the 1.0.13 image ran but **not** that the region-skip branch did: the retained unified
log has no `i915` line and neither `stolen region withheld` nor `ring pinned`, so the `drm_info` output is
simply not in the capture. No mechanism from "no stolen region" to "zeroed context mutex" is demonstrated by
the two diffs, and nothing excludes one either.

**Lesson:** a candidate that fails earlier is not a stronger explanation; it is a new question. Park it,
record it, and do not re-arm it unchanged.

## 19 October — Phase 10: the panic named (1.0.14)

The 1.0.13 stack is a **context use-after-free**: the breadcrumb completion path reaches a context that
timeline retirement has already freed. It matches the upstream Linux patch *"drm/i915/gt: Fix ce
use-after-free in signal_irq_work()"*
(https://lists.openwall.net/linux-kernel/2026/09/16/2418), which the port adapts to the FreeBSD RCU list
primitives available under LinuxKPI.

The correction:

* every breadcrumb-owned request reference also holds its context and, for parallel requests, a separate
  parent context reference, with lifetime running through deferred GuC priority retirement, fence
  notification or cancellation;
* the already-completed enqueue path acquires ownership **before** publishing the fence's `SIGNALED` bit,
  since timeline retirement can act immediately on that bit;
* the signaler iterator snapshots the next context before releasing the current one, and the request
  iterator snapshots its next entry before a losing signal path can drop the last request reference.

Local validation with the **actual** old enqueue/worker/GuC functions reproduces a heap-use-after-free; the
corrected functions pass **90,000** ASan/UBSan lifetimes (publication race, parent/child, cancel, bulk
remove, competing signal, duplicate signaling) with zero live objects. A separate request-iterator UAF was
reproduced and fixed. The 438-object link keeps the frozen 1.0.8 undefined-symbol set. UUID
`9B0192B5-…`, SHA-256 `80c3fe8b…`.

**Hardware-unverified.** The one 1.0.14 trial (18:39:51) panicked earlier and elsewhere: a null-page fault
(`CR2=0`, uptime 9.12 s) in `VoodooI2CTouchscreenHIDEventDriver::forwardReport`. The same function and
offset appear in four older panic reports from 17–18 October, predating 1.0.14, and NMXGPU 1.0.14 was in
loaded modules but **not** in the faulting backtrace — a separate pre-existing HID fault, not a verdict on
the GPU fix. Separately, the installer now clears the NVRAM `nullmoth-gpu-attaching` marker that a failed
attach leaves behind, which had been parking every subsequent boot.

**Lesson:** a fix you can reproduce locally is still not a hardware fix. The only thing that settles it is a
target boot that reaches the GPU work — and 1.0.14's first trial never got that far.

## 19 October — Phase 11: the desktop works (1.0.15)

Kernel 1.0.15 replaces the broad stolen-region skip with a **precise reservation**: an IOKit bridge reads
the firmware console address, pitch and dimensions, and the stolen allocator starts *after* that image
instead of offering it to i915. Invalid or missing console data, or a console outside usable DSM, refuses
the stolen allocator rather than risking overlap. The local model proves allocation exclusion; it does not
prove that a specific earlier failed ring overlapped this image on hardware.

With the guarded HID null-guard, the 22:38 boot produced the project's first **usable GPU-composited
desktop**: WindowServer submits real GPU work and reads back correctly — 24 fence + 32 readback
completions, all clean — with no GPU hang (`windowserver-result-224146`). Host-pointer imports are still
refused, so surfaces use the private-image/copy path.

**The blocker changed shape completely.** It is no longer "the GPU hangs" but "the desktop is choppy":
CoreAnimation fence timeouts, publication stalls and GPU completion waits.

**Lesson:** the hardest part of a port is not the first working result, it is knowing which problem you are
solving. For six candidates the question was "does the GPU execute?"

## 19–20 October — Phase 12: reporting, smoothness and the peripherals (0.2.5 → 0.3.19)

* **Driver-queried GPU identity (framebuffer 0.2.5 + plugin 0.3.6).** The Metal name comes from
  `VkPhysicalDeviceProperties.deviceName` and heap sizes from `VkPhysicalDeviceMemoryProperties` — no
  fixed name or size injection, and the unpublished fixed-size config was removed. The framebuffer
  publishes the queried `model` and `VRAM,totalMB` to the PCI registry without ever enlarging the physical
  scanout mapping. System Information now reports `Intel(R) Graphics (MTL)` and the queried shared
  capacity — 12288 MB before the 4 GiB backend cap, 4096 MB after it.
* **0.3.11's optimized cache path.** CPUID-gated `CLFLUSHOPT` batching with the same before/after
  `MFENCE` boundaries, targeting the measured serial cache-maintenance cost. The recorded readback total
  fell from **22.4 ms to 5.0 ms** (report `135231`) — different startup captures, not a controlled
  frame-rate benchmark.
* **0.3.12** corrected the swizzled-texture-view identity/storage separation and the per-scanline readback
  copy (one `memcpy` for a tight layout).
* **0.3.14** supplied the captured 128-byte BOE panel EDID (1920×1080, 294×165 mm ≈ 13.3") and refreshed
  the About This Mac icon cache.
* **0.3.15 / 0.3.16** published the native `BRT6` brightness route and the gamma/Night Shift path
  (framebuffer 0.2.8) plus ALC295 audio via AppleALC 1.9.8 (compatibility device-id `ca510000`,
  layout-id 33). No global F6/F7 remap, no keyboard or touchpad kext replacement.
* **0.3.17 / 0.3.18 / 0.3.19** followed with the native key publication, the colour-transaction path and
  the scanout copy, plus a device-lifetime Vulkan pipeline cache and the 4 GiB ANV backend cap.
* The remaining presentation problems (7 CoreAnimation fence timeouts, a 172 ms publication stall, a
  2.39 s GPU completion wait) were chased by measurement rather than guesswork. The desktop is now smooth.

## 20–21 October — Phase 13: the unified candidate (tablet, subpixel text, boot UI, Copilot, persistent accelerated boot)

The project moved from "a graphics port with peripherals bolted on" to **one staged candidate** carrying
graphics, wireless, audio, input, tablet, text rendering and the boot UI, with each subsystem's real
validation state recorded separately.

* **Tablet mode.** A scoped OpenCore HIDD method-rename patch plus an SSDT (V3 patches the supplied DSDT
  directly and uses original recovery tables); a live firmware reader 0.2.0 compiled and tested under
  ASan/UBSan with mocked firmware; device-local PS/2 gates with held-state release, quarantine and
  stale-data restoration. The policy is a 500 ms debounce, stale-signal fail-open, flat hold and an
  editable-field-only on-screen keyboard. The original ACPI collector silently returned empty data and was
  corrected — empty or zero-gated firmware readings are **not** tablet mode. The user confirmed automatic
  suppression of the built-in keyboard/touchpad while folded, restoration when unfolded, and that external
  input and the touchscreen remain usable. A guided Linux capture now verifies the Intel HID switches and
  accelerometer axes across 50 samples; the Dell WMI switch remains false, and macOS still has no qualified
  live ISH sensor/mode publisher.
* **Subpixel LCD text.** A private FreeType 2.14.3 with filtered LCD rendering (RGB, RGB_V, BGR, BGR_V),
  a bounded 4 MiB coverage atlas with eviction and orientation-generation invalidation, per-channel blending
  in linear light over a known opaque background, and a CoreText-shaped-run frontend that keeps native
  glyph positioning and font fallback instead of guessing character-to-glyph mappings. A process-local native
  `CTLineDraw` interposer passes sanitized and release runtime tests through an ordinary caller with
  clipping and context-lifetime validation; the native AppKit glyph route and deferred transparent CPU
  text-image composition also pass, including San Francisco variable fonts. System-app interception and
  component-alpha compositor integration are **not** complete, and the attempted late dyld loader was
  rejected after source and runtime verification showed that API is a no-op.
* **Boot UI — macOS Golden Gate.** The GoldenGateExt theme at revision `d43617a`, `PickerMode=External`,
  `PickerVariant=Acidanthera\GoldenGateExt`, `PickerAttributes=147`, `HideAuxiliary=true`, with
  `ResetNvramEntry` kept as an auxiliary entry revealed by Spacebar. OpenCanopy and AudioDxe come from the
  exact active OpenCore build (1.0.9 DEBUG, 10 October 2026), hash-checked against Dortania's build manifest
  and compared byte-for-byte to the active loader; the core loader and OpenRuntime are not replaced. The
  official `OCEFIAudio_VoiceOver_Boot.mp3` is included, with the Dell's `PciRoot(0x0)/Pci(0x1F,0x3)` HDA
  path, codec 0, all enumerated outputs and a −15 dB gain ceiling; the OS volume/mute preferences are not
  forcibly rewritten, so a muted system can stay silent.
* **Copilot → Command.** Confirmed working after a reboot through the USB. It is VoodooPS2Keyboard
  `Info.plist` **macro inversion**, not a userspace remapper or a firmware flash. The canonical artifact is
  `port-artifacts/copilot-command/VoodooPS2Keyboard.kext`, retained and validated idempotently by
  `copilot-sources/retain.py` (and `--check`), with `copilot-sources/test-macros.py` exercising the actual
  upstream inversion code. The default `Swap command and option` must stay false, existing profile macros
  and unrelated settings are preserved, and the touchpad apply/restore scripts do not replace the PS/2
  bundle. A new keyboard implementation requires explicit porting and validation, not blind profile copying.
* **Persistent accelerated boots.** OpenCore NVRAM Delete+Add refreshes `nullmoth-gpu-once` and
  `nullmoth-panel-once` (Apple GUID `7C436110-AB2A-4BBB-A880-FE41995C9F82`) so every normal USB boot is
  accelerated; the driver's attach-failure guard and the explicit manual recovery profiles remain.
  `graphics-mode-sources/prepare.py` creates and validates the policy, and the GraphicsModes profiles and
  switch script are regenerated (hash-guarded) rather than overwritten with an old profile.

## 21–22 October — Phase 14: the AWDL radio and the native backend (0.1.2 → 0.1.4)

The AWDL source lineage and the missing radio paths were audited and a diagnostic collector added; the
pinned OWL protocol engine compiles for Intel and passes host sanitizer tests for sync windows, peer
lifecycle and action frames.

* **0.1.2 (radio)** — the selected USB driver for the next hardware qualification. The radio API is
  implemented, automatic RF transmission is off, and Dell hardware validation is still pending.
  **AirDrop is not functional.**
* **0.1.3 (peer)** — the peer policy is unbound and the active EFI stays byte-identical. AirDrop is still
  not functional.
* **0.1.4 (native backend)** — a *local development build*, not an installable AirDrop release. The x86_64
  Sequoia kext compiled all 74 units, linked and passed ad-hoc signature verification; native packet and
  HAL-body fixtures passed ASan/UBSan with external kernel/firmware primitives substituted. Implemented:
  separately owned secondary PHY/P2P/listener contexts, a bounded raw injection queue, a native
  prepared-packet adapter that preserves AWDL sequence and avoids duplicate encapsulation, original-packet
  ownership separate from the radio DMA copy, firmware response/reclaim separation, reset completion after
  DMA quiescence, and known-peer RX validation with replay rejection. Uncertain firmware ownership poisons
  the transaction until reset instead of recycling identifiers. **Not connected:** the native interface
  factory/control handlers, native action input metadata, peer discovery, the native dequeue
  worker/completion adapter, and the runtime channel/deadline timers; no native frontend caller invokes the
  new bind entry point, and shared PHY/listener operation has not been qualified on the Dell.
  **AirDrop does not work in this checkpoint**, and default hardware activity is off.

Sources are pinned to `itlwm` `d695c971420cc8af0daa286dcd3297e8236c0a63` with the MTL fixes
`05b19dc81db6aedcf4058525981f7326f57c04e2`; the native ABI targets `IO80211Family` 1200.13.1 from macOS
15.8.1 build 24H32.

## 22 October — Phase 15: the macOS 27 "Golden Gate" standalone boot program

**macOS 27 "Golden Gate"** is Apple's twenty-third major macOS release, released 14 September 2026 — the
**first macOS that is Apple Silicon only** and the **last to ship Rosetta 2**. The ambition is **Snapintosh
for the Dell**: boot macOS 27 on the Intel Meteor Lake machine *directly from firmware*, with no Linux host
and no Sequoia loader, which on this hardware means an **ARM-to-x86 translation layer beneath the kernel**.

The work lives in `macos27-standalone/` (mirrored from the Corsair-backed build volume) and has already
produced three published versions.

* **0.0.1 — the standalone UEFI preflight.** An x86_64 UEFI application that runs with no Linux and no
  Sequoia: it inventories firmware RAM and GOP dimensions, admits exactly four hash-pinned macOS 27.0
  build 26A428 / Mac14,3 inputs (`bootkc`, `dtree`, `sptm`, `txm`), validates ARM64 MH_FILESET
  load-command bounds, and stops with an explicit "ARM-to-x86 execution runtime is UNIMPLEMENTED"
  message. Virtual-firmware tests pass, including rejection of a corrupted boot file.
* **0.0.2 — an A64 execution core inside the EFI binary.** A portable interpreter executes a bounded
  synthetic A64 program: scalar move-wide, ADD/SUB, logical shifted-register operations, NZCV, ADR/ADRP,
  direct/conditional/register branches, calls/returns, integer loads/stores, NOP and BRK, with explicit
  private buffers that resolve guest addresses under permission and overflow checks. Stores validate every
  byte before changing memory. Invalid maps, unsupported instructions, bad addresses and instruction
  budgets terminate execution; an unsupported system instruction is never treated as a successful no-op.
  Verified with 66 execution/fault checks, a byte-for-byte QEMU comparison and virtual UEFI execution.
* **0.0.3 — privileged state and the real SPTM entry.** A checked ARM64 MH_EXECUTE loader (813 boundary
  checks), selected non-trapping EL2 state (OSLAR, DAIF masks, CurrentEL, VBAR, reset HCR read) and an
  independent 19-instruction privileged probe whose 80 result bytes match QEMU with virtualization enabled.
  After admission the probe rereads and rehashes SPTM, maps it into private memory, executes its first
  branch and four setup instructions, then **intentionally stops at the first boot-argument read** — no
  boot-argument region is supplied. The guest base `0x800000000` is a bounded probe address, not a
  qualified Darwin board layout. A privileged ARM fault in the EL2 test was traced and corrected, and a
  cached privilege-state bug in the Unicorn fork was fixed. Verified with 85 execution checks, 813 loader
  checks, QEMU comparisons and virtual UEFI execution: 69 instructions and 80 QEMU-matched result bytes,
  plus a separate run that rejects a one-byte-altered device tree.
* **The broader engine.** Intel's `unicorn-for-efi` has been compiled on the build volume (80 source
  files, 28 unresolved firmware helpers, not runtime qualified) and linked into a standalone x64 EFI
  application with a firmware service bridge. The first firmware run caught a calling-convention mismatch
  before the JIT ran; after correcting the EFI calls to the Microsoft x64 ABI the linked engine runs the
  scalar ARM payload through its JIT inside x64 UEFI with all 80 result bytes matching QEMU, and the
  selected EL2 tests pass. The next step is connecting the engine to the reference's real boot arguments
  and memory layout from a hash-verified snapshot captured before the reference CPU executed anything.

**What is not there, stated plainly.** A complete privileged ARM runtime, the MMU and monitor memory
management, exception delivery, SIMD/FP, atomics and PAC, the Apple interrupt/timer/device models, SPTM
initialisation to completion, TXM, the Apple kernel and the desktop. The console explicitly stops at that
boundary rather than reporting success.

**The USB now holds macOS 27.** The small FAT volume carries `EFI/Tools/Dell27Preflight.efi` (0.0.3,
SHA-256 `931ade60…`) and the four pinned boot inputs; the main exFAT volume holds the source, the evidence
and `macOS27-Shell`, an older *hosted* QEMU shell reference. Both original SanDisk partitions were erased
with user authorization after the full Corsair backup was verified, and **no Sequoia EFI was restored** and
no default `EFI/BOOT/BOOTX64.EFI` was installed. The small FAT volume has a Microsoft Basic Data GPT type,
so automatic removable boot discovery is unqualified; Dell **Boot from File** selects the Tools
application. No Linux host is used and the Dell has not yet qualified either execution path.

**Lesson:** the honest boundary is the deliverable. Every version so far stops explicitly at the edge of
what it can actually do, and says so on the console, instead of reporting a success it has not earned.

---

## 23–30 October — Phase 16: native recovery services and WindowServer

The full ARM64-on-x86 UEFI engine progressed from kernel initialization through HFS recovery userland
and native WindowServer startup. The current run has reached WindowServer's server loop with
CoreAnimation and HID threads; actual framebuffer captures remain black. Component correctness and
memory/presenter work are qualified independently of visible installer output.

On 30 October the current EFI, matching source/upstream overlay and build evidence were published
alongside the working Tahoe0.3.21 graphics/audio and Wi-Fi packages. See the dedicated
[macOS27 roadmap](#macos27) for the exact candidate, evidence, current waits and acceptance gates.

**Lesson:** native userland and a running display server are measurable progress. Display adoption,
real pixels, input and storage must still be verified rather than inferred from those processes.

---

# Method

## Evidence and corrected status

The following table preserves the **22 October graphics evidence snapshot**. Current Tahoe and
macOS27 status is recorded at the top of this document and in the dedicated macOS27 section.

| Milestone | Current evidence | Status |
|---|---|---|
| Real GPU identification | macOS probe and the laptop's own Linux report agree on 7D45 | **Confirmed** |
| Application GPU output | 22 cases / 26,000 frames / 80,020 pools; report `readback-20261019-114424` | **Working**, scoped to that matrix |
| Physical framebuffer and scanout adoption | Custom framebuffer loads, describes 1920×1080, reaches fixed scanout, supplies the captured EDID | **Working** |
| Accelerator/display registration | NMXAccel 0.2.9 verifies pipe 0, phase 2 complete, pipe active | **Confirmed initialization** |
| Desktop workload execution | 24 fence + 32 readback completions, all clean, no GPU hang | **Working** |
| Presentation quality | 7 CoreAnimation fence timeouts, 172 ms publication stall, a 2.39 s GPU completion wait — all measured | **Smooth, with residual latency** |
| GPU reporting | `Intel(R) Graphics (MTL)`, queried shared capacity | **Working** |
| Peripherals | Wi-Fi, Bluetooth, audio, Night Shift, brightness, touchscreen confirmed on the Dell | **Working** |
| Touchpad, AWDL/AirDrop, tablet publication, subpixel text | Implemented with local fixtures; the radio data plane and live ISH publisher are missing | **WIP** |
| Sleep/wake, external outputs, video decode | No relevant passing evidence | **Deferred** |

`desktop exit=0` only says the small window test ran; it is not proof of acceleration. `NMXDisplay=armed` is
driver state, not a pixel-correctness result. A fallback-boot failure does not retest the active GPU.

Machine-readable evidence: `research/desktop-roadmap-evidence.json`.

## What the research changed

1. **Readback was the first observed failure, not the proven cause.** Submission #5's fence never
   completed while #3 passed; the trace never established that they shared an image. That framing is what
   kept the search on the *first uncompleted* submission.
2. **Resource synchronization had to be audited before adding global flags.** `nvmtl_vk_image_read`
   records no explicit transfer-write → host-read dependency. Adding a global lock around waits could
   create a new deadlock and is not a substitute for that audit. (Addressed: the plugin now has a
   CoreAnimation-shaped event path and a copy-out fallback that completes.)
3. **Lifetime/reuse tests are separate from synchronization tests.** Pointer equality is not reuse while
   pending; Vulkan forbids freeing pending command buffers. (Addressed: fresh pools, immediate destroy and
   quarantine on uncertainty.)
4. **The laptop's Linux evidence constrains kernel experiments.** Stepping B0, `has_llc=no`,
   `has_snoop=yes`, `has_flat_ccs=no` and the firmware versions are reference evidence, not a substitute
   for a matching rendering test — and Xe2/Xe3 flat-CCS remedies and Panther Lake VF power/firmware
   settings must not be copied onto this device.
5. **Negative experiments are recorded accurately.** `INTEL_DEBUG=stall` was enabled on the failing 0.2.5
   boot and did not cure the hang. It stays a recorded negative, not a proposed fix.

## Implementation sequence and promotion criteria

### A. Application gate — **done**

22 cases, 11 IOKit and 11 direct, 1,000 iterations each, exact-pixel comparison, pool accounting
`created = destroyed = completed + abandoned`. The suite refuses to arm a desktop boot unless the loaded
kernel/plugin/libdrm hashes match the package, the boot UUID is current, and the parser sees no
timestamped kernel GPU-hang, TLB-timeout/reset or fault events. The parser excludes log-filter headers
and command metadata, which previously produced a false-positive hang match.

### B. Focused readback correctness — **done**

The transfer-write → host-read dependency, layout/subresource validation, image/staging generation
tracing and the quarantine policy are in the tree. The remaining presentation work is latency, not
correctness: the copy-out path completes and the readbacks are correct.

### C. Desktop acceptance — **reached**

WindowServer is GPU-composited, submits real work and reads back correctly with no hang, and the desktop
presents smoothly. A visible software-fallback desktop is still **not** a pass, and the acceptance
evidence is the fence/readback completion log plus the measured presentation numbers — not a screenshot.

### D. Apple-side subsystems — **current work**

| Area | Next step | Evidence required |
|---|---|---|
| AWDL / AirDrop | Qualify the 0.1.2 radio bridge on the Dell, then connect the 0.1.4 native factory, peer discovery and dequeue worker | A peer discovered, an action frame delivered, and a real file transferred |
| Touchpad | Fix the ELAN device so it is usable, not merely readable | Pointer, tap and two-finger scroll confirmed on the Dell |
| Tablet mode | Publish a live ISH sensor/mode source for macOS | A mode switch observed and the input suppression policy acted on |
| Subpixel text | Complete the system-app interception and the GPU-backed CA layer | A real app rendering through the filtered path on the Dell |
| Peripherals | Target-validate brightness keys, hotkeys, GPIO and sleep/wake | A recorded pass per item, not a compiled fixture |

### E. Broaden support after these close

Then test ordinary Metal/OpenGL applications, memory pressure, repeated application exit, lock/unlock and
multiple displays. Modesetting, hardware cursor, video acceleration and power management remain
separately scoped work. Each needs its own evidence; passing compute/render tests does not certify them.

### F. macOS 27 "Golden Gate" — the standalone boot programme

**Historical dependency plan (22 October).** Implemented stages are now tracked in the
[dedicated current roadmap](#macos27); the original plan below remains as engineering history.

Two tracks, and they must not be confused.

**F1. The standalone runtime** (`macos27-standalone/`). The order below is the order of dependency, and
each step is gated on the previous one being verified rather than merely compiling:

| Step | Done when |
|---|---|
| A64 scalar core in the EFI binary | Bounded payloads match pinned QEMU byte-for-byte inside x64 OVMF |
| Privileged EL2 state and the MH_EXECUTE loader | The privileged probe matches QEMU with virtualization enabled; unsupported privilege transitions stop rather than report success |
| SPTM entry and the boot-argument handoff | The real SPTM image initialises past its first boot-argument read, with the arguments supplied from a hash-verified snapshot |
| The MMU and monitor memory management | Guest page tables and guest-physical translation are exercised against a reference layout |
| Exception delivery, timers and the Apple device models | A faulting guest instruction reaches the expected handler, and the guest's timer interrupts arrive |
| TXM and the Apple kernel | The kernel begins executing under translation |
| The desktop | A usable macOS session on the Intel panel |

Every step keeps the same rule the graphics work follows: **an unsupported path stops explicitly**. A
build that compiles is not a step that passed.

**F2. The 24H32 native-integration re-anchoring.** The AWDL/`IO80211Family` ABI, the CoreText/FreeType
text path and the ISH tablet sensors are each versioned against the running OS, so moving to Golden Gate is
a re-derivation per framework, not a version bump. The pinned references are `research/iwlwifi-api83/`
(the Linux `iwlwifi` API-83 source) and `research/airportitlwm-tahoe/` (a qualified upstream reference on
Tahoe 26.6.2 / 26.7 with AppleVTD, AWDL/P2P, AirPlay and Screen Mirroring).

## Immediate deliverables and boundaries

1. **The macOS 27 standalone runtime** — privileged ARM execution, the MMU and monitor memory management,
   exception delivery, SPTM initialisation to completion, then the Apple kernel. The larger of the two
   programmes by far.
2. **The touchpad** — the last everyday blocker on the Dell.
3. **AWDL radio qualification**, then the native AirDrop data plane.
4. **A live ISH publisher** for tablet mode and auto-rotation.
5. **System-wide subpixel text deployment** with the GPU-backed CA layer.
6. **Sustained use** — soak, sleep/wake and external displays, still unverified.

Boundaries: do not rewrite historical backups, frozen passing graphics baselines or old checkpoints to
pretend they contained a later fix; reapplying an old EFI requires applying the confirmed Copilot change
to a new working copy. Do not mark the whole candidate complete from a single subsystem test. Do not
treat empty or zero-gated firmware readings as tablet mode, and do not claim the diagnostic-only helper
implements automatic features. No automatic reboot is requested by any script.
