FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        curl \
        gnupg \
        ca-certificates \
        lsb-release \
    && curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key \
        | gpg --dearmor -o /usr/share/keyrings/llvm.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/llvm.gpg] http://apt.llvm.org/$(lsb_release -cs)/ llvm-toolchain-$(lsb_release -cs)-19 main" \
        > /etc/apt/sources.list.d/llvm.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        autoconf \
        cmake \
        ninja-build \
        gcc \
        g++ \
        linux-tools-common \
        linux-tools-generic \
        libtool \
        llvm-19-dev \
        lld-19 \
        clang-19 \
        libomp-19-dev \
        libopenmpi-dev \
        openmpi-bin \
        git \
        make \
        imagemagick \
        ffmpeg \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-19 10 --slave /usr/bin/clang++ clang++ /usr/bin/clang++-19 \
    && update-alternatives --install /usr/bin/opt opt /usr/bin/opt-19 10 \
    && update-alternatives --install /usr/bin/lld lld /usr/bin/lld-19 10 \
    && update-alternatives --install /usr/bin/llvm-symbolizer llvm-symbolizer /usr/bin/llvm-symbolizer-19 10

RUN ln -s /usr/bin/convert /usr/bin/magick

ENV DEBIAN_FRONTEND=