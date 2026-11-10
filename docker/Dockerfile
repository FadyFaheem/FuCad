FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive \
    PIXI_HOME=/opt/pixi \
    PIXI_VERSION=v0.68.1 \
    PATH=/opt/pixi/bin:$PATH

RUN apt-get update && apt-get install -y --no-install-recommends \
        bash \
        binutils \
        bzip2 \
        build-essential \
        ca-certificates \
        coreutils \
        curl \
        file \
        findutils \
        git \
        gzip \
        patch \
        patchelf \
        pkg-config \
        tar \
        xz-utils \
        zstd \
    && rm -rf /var/lib/apt/lists/*

RUN curl -fsSL https://pixi.sh/install.sh | bash

COPY build-appimage.sh /usr/local/bin/build-appimage.sh
RUN chmod +x /usr/local/bin/build-appimage.sh

WORKDIR /src
ENTRYPOINT ["/usr/local/bin/build-appimage.sh"]
