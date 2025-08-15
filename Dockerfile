FROM debian:13

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get -y -qq --no-install-recommends install \
        cmake \
        make \
        zstd \
        unzip \
        libzstd-dev \
        clang-19 \
        libclang-rt-19-dev \
        libomp-19-dev \
        clang-format-19 \
        llvm-19 \
        llvm-19-dev \
        openmpi-bin \
        libopenmpi-dev \
        openjdk-25-jre openjdk-25-jdk

ENV PMIX_MCA_gds="hash"
ENV OMPI_MCA_memory="^patcher"

# Compile CoVer
COPY ./CoVer /tmp/cover_src
WORKDIR /tmp/cover_src
RUN mkdir build && cd build && cmake .. -DCMAKE_PREFIX_PATH=/opt/cover && cmake --build . --target install
