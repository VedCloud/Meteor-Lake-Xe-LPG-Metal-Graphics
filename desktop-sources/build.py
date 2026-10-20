
#!/usr/bin/env python3

import plistlib
import shutil
import subprocess
from pathlib import Path


# ---------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------

ROOT = Path(__file__).resolve().parent.parent

SOURCE_DIR = ROOT / "NullMoth/kexts/NMXVGAFB"
DESKTOP_SOURCES = ROOT / "desktop-sources"
BUILD_DIR = ROOT / "desktop-build"
PACKAGE_DIR = ROOT / "latitude-desktop"

MACOS_VERSION = "15.0"
ARCHITECTURE = "x86_64"
KEXT_VERSION = "0.2.3"

# Intel Meteor Lake Xe-LPG PCI device IDs
SUPPORTED_GPU_IDS = (
    "0x7d458086",
    "0x7d558086",
)

KEXTS = {
    "LatitudeMTLFB": {
        "subdir": "Library/Extensions",
        "objects": ["nmxvgafb", "nmx_virtio_gpu"],
        "io_class": "NMXIntelVGAFB",
    },
    "LatitudePanelClaim": {
        "subdir": "EFI-Kexts",
        "objects": ["LatitudePanelClaim"],
        "io_class": "LatitudePanelClaim",
    },
}

SOURCES = {
    "nmxvgafb": SOURCE_DIR / "nmxvgafb.cpp",
    "nmx_virtio_gpu": SOURCE_DIR / "nmx_virtio_gpu.cpp",
    "LatitudePanelClaim": DESKTOP_SOURCES / "LatitudePanelClaim.cpp",
}


# ---------------------------------------------------------------------
# Utilities
# ---------------------------------------------------------------------

def run_command(command, log_path=None):
    """Run a command and raise an error if it fails."""

    if log_path is None:
        subprocess.run(command, check=True)
        return

    with log_path.open("w") as log:
        result = subprocess.run(
            command,
            stdout=log,
            stderr=subprocess.STDOUT,
        )

    if result.returncode != 0:
        print(f"\nBuild failed. Log: {log_path}")
        print(log_path.read_text(errors="replace")[-6000:])
        raise SystemExit(result.returncode)


def get_sdk_path():
    return subprocess.check_output(
        ["xcrun", "--show-sdk-path"],
        text=True,
    ).strip()


def get_compiler_flags(sdk_path):
    kernel_headers = (
        Path(sdk_path)
        / "System/Library/Frameworks/Kernel.framework/Headers"
    )

    return [
        "-arch", ARCHITECTURE,
        f"-mmacosx-version-min={MACOS_VERSION}",
        "-fapple-kext",
        "-mkernel",
        "-nostdinc",
        f"-I{kernel_headers}",
        "-I" + str(DESKTOP_SOURCES),
        "-DKERNEL",
        "-DKERNEL_PRIVATE",
        "-DDRIVER_PRIVATE",
        "-DAPPLE",
        "-DNeXT",
        "-DLATITUDE_MTL_PANEL",
        "-std=c++17",
        "-fno-rtti",
        "-fno-exceptions",
        "-fno-builtin",
        "-fno-common",
        "-Wall",
        "-Wno-unused-parameter",
        "-Wno-inconsistent-missing-override",
        "-O2",
    ]


# ---------------------------------------------------------------------
# Compilation
# ---------------------------------------------------------------------

def compile_sources(flags):
    print("\n=== Compiling framebuffer sources ===")

    for name, source_path in SOURCES.items():
        object_path = BUILD_DIR / f"{name}.o"
        log_path = BUILD_DIR / f"{name}.log"

        print(f"Compiling {name}...")

        command = [
            "xcrun", "clang++",
            *flags,
            "-c", str(source_path),
            "-o", str(object_path),
        ]

        run_command(command, log_path)

    print("All sources compiled successfully.")


# ---------------------------------------------------------------------
# Kext metadata
# ---------------------------------------------------------------------

def create_info_plist(name, configuration):
    bundle_id = f"com.nullmoth.{name}"

    libraries = {
        "com.apple.kpi.iokit": "8.0",
        "com.apple.kpi.libkern": "8.0",
        "com.apple.kpi.mach": "8.0",
        "com.apple.iokit.IOPCIFamily": "1.0.0",
    }

    personality = {
        "CFBundleIdentifier": bundle_id,
        "IOClass": configuration["io_class"],
        "IOProviderClass": (
            "LatitudePanelClaim"
            if name == "LatitudeMTLFB"
            else "IOPCIDevice"
        ),
        "IOMatchCategory": "IOFramebuffer",
        "IOProbeScore": 60000,
    }

    if name == "LatitudePanelClaim":
        # Support both Meteor Lake Xe-LPG GPU device IDs.
        personality["IOPCIMatch"] = " ".join(SUPPORTED_GPU_IDS)
    else:
        libraries.update({
            "com.apple.iokit.IOGraphicsFamily": "1.0.0",
            "com.apple.kpi.bsd": "8.0",
        })

    info = {
        "CFBundleExecutable": name,
        "CFBundleIdentifier": bundle_id,
        "CFBundleName": name,
        "CFBundlePackageType": "KEXT",
        "CFBundleVersion": KEXT_VERSION,
        "CFBundleShortVersionString": KEXT_VERSION,
        "CFBundleInfoDictionaryVersion": "6.0",
        "OSBundleLibraries": libraries,
        "IOKitPersonalities": {
            name: personality,
        },
    }

    if name == "LatitudePanelClaim":
        info["OSBundleRequired"] = "Root"

    return info


# ---------------------------------------------------------------------
# Kext linking and signing
# ---------------------------------------------------------------------

def build_kext(name, configuration):
    print(f"\n=== Building {name}.kext ===")

    kext_path = (
        PACKAGE_DIR
        / configuration["subdir"]
        / f"{name}.kext"
    )

    executable_dir = kext_path / "Contents/MacOS"
    executable_dir.mkdir(parents=True, exist_ok=True)

    executable_path = executable_dir / name

    object_paths = [
        BUILD_DIR / f"{object_name}.o"
        for object_name in configuration["objects"]
    ]

    link_command = [
        "xcrun", "clang++",
        "-arch", ARCHITECTURE,
        f"-mmacosx-version-min={MACOS_VERSION}",
        "-fapple-kext",
        "-nostdlib",
        "-Xlinker", "-kext",
        "-lkmodc++",
        "-lkmod",
        "-lcc_kext",
        *map(str, object_paths),
        "-o", str(executable_path),
    ]

    print("Linking...")
    run_command(link_command)

    print("Generating Info.plist...")
    info = create_info_plist(name, configuration)

    plist_path = kext_path / "Contents/Info.plist"
    plist_path.write_bytes(plistlib.dumps(info))

    print("Signing...")
    run_command([
        "codesign",
        "--force",
        "--sign", "-",
        str(kext_path),
    ])

    print("Verifying signature...")
    run_command([
        "codesign",
        "--verify",
        "--strict",
        str(kext_path),
    ])

    print(f"Successfully built {kext_path}")


# ---------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------

def main():
    BUILD_DIR.mkdir(parents=True, exist_ok=True)

    sdk_path = get_sdk_path()
    flags = get_compiler_flags(sdk_path)

    print("Meteor Lake Xe-LPG Framebuffer Builder")
    print(f"SDK: {sdk_path}")
    print(f"Architecture: {ARCHITECTURE}")
    print(f"Supported GPU IDs: {', '.join(SUPPORTED_GPU_IDS)}")

    compile_sources(flags)

    for name, configuration in KEXTS.items():
        build_kext(name, configuration)

    print("\nAll framebuffer kexts built successfully.")


if __name__ == "__main__":
    main()

