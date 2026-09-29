# CognitaOS Dockerfile
FROM ubuntu:24.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    clang \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /cognitaos

COPY . .

RUN mkdir build && cd build && \
    cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release && \
    ninja

# Runtime image
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /cognitaos/build/lib/libcognita.so /usr/local/lib/
COPY --from=builder /cognitaos/include/cognita /usr/local/include/cognita

RUN ldconfig

ENV LD_LIBRARY_PATH=/usr/local/lib

CMD ["bash"]
