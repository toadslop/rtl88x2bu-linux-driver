# L0 CI image: pinned v6.12.9 Rust-enabled kernel + driver build toolchain (T6).
# Build from repo root:
#   docker build -f .github/docker/l0/Dockerfile -t rtl88x2bu-l0:v6.12.9 .
# See docs/rust-migration/dev-environment.md#ci-l0-image

FROM ubuntu:24.04

ARG KERNEL_TAG=v6.12.9

ENV DEBIAN_FRONTEND=noninteractive \
	KDIR=/opt/linux \
	LIBCLANG_PATH=/usr/lib/llvm-18/lib \
	KERNEL_TAG=${KERNEL_TAG}

RUN apt-get update && apt-get install -y --no-install-recommends \
		bc \
		bison \
		build-essential \
		busybox-static \
		ca-certificates \
		clang-18 \
		cpio \
		curl \
		dwarves \
		flex \
		git \
		libclang-dev \
		libelf-dev \
		libssl-dev \
		lld-18 \
		llvm-18 \
		make \
		pkg-config \
		qemu-system-x86 \
	&& rm -rf /var/lib/apt/lists/*

# Kernel LLVM=1 expects unsuffixed llvm-* / ld.lld / clang names.
RUN ln -sf /usr/bin/ld.lld-18 /usr/bin/ld.lld \
	&& ln -sf /usr/bin/clang-18 /usr/bin/clang \
	&& ln -sf /usr/bin/clang++-18 /usr/bin/clang++ \
	&& for f in /usr/bin/llvm-*-18; do \
		base="$(basename "$f" -18)"; \
		[ -e "/usr/bin/${base}" ] || ln -sf "$f" "/usr/bin/${base}"; \
	done

# Rust toolchain (edition 2024 requires rustc >= 1.85; Linux 6.12.x min bindgen 0.65.1).
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs \
	| sh -s -- -y --default-toolchain 1.85.0 --profile minimal \
	&& . "$HOME/.cargo/env" \
	&& rustup component add rust-src rustfmt clippy \
	&& cargo install bindgen-cli --version 0.65.1 --locked

ENV PATH="/root/.cargo/bin:${PATH}"

COPY scripts/ci/build-pinned-kernel.sh /usr/local/bin/build-pinned-kernel.sh
RUN chmod +x /usr/local/bin/build-pinned-kernel.sh \
	&& KERNEL_TAG="${KERNEL_TAG}" INSTALL_DIR=/opt/linux build-pinned-kernel.sh

WORKDIR /driver
