#!/bin/bash
set -euo pipefail
candidate=$(cd "$(dirname "$0")" && pwd)
tools='/Volumes/Corsair 1TB NVMe/iGPU Port/toolchain'
export CARGO_HOME="$tools/cargo" RUSTC="$tools/rustup/toolchains/stable-aarch64-apple-darwin/bin/rustc" RUSTDOC="$tools/rustup/toolchains/stable-aarch64-apple-darwin/bin/rustdoc" CARGO_TARGET_DIR="$candidate/build/target"
exec "$tools/rustup/toolchains/stable-aarch64-apple-darwin/bin/cargo" test --offline --manifest-path "$candidate/translator/translator/Cargo.toml" --test latitude_a8 -- --nocapture
