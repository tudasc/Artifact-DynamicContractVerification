FROM debian:13

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get -y -qq --no-install-recommends install \
        cmake \
        make \
        zstd \
        git \
        python3 \
        python3-venv \
        unzip \
        nano \
        libzstd-dev \
        clang-19 \
        libclang-rt-19-dev \
        libomp-19-dev \
        clang-format-19 \
        llvm-19 \
        lld-19 \
        llvm-19-dev \
        openmpi-bin \
        libopenmpi-dev \
        openjdk-25-jre openjdk-25-jdk

RUN ln -s /usr/bin/clang-19 /usr/bin/clang
RUN ln -s /usr/bin/clang++-19 /usr/bin/clang++
RUN ln -s /usr/bin/llvm-link-19 /usr/bin/llvm-link
RUN ln -s /usr/bin/opt-19 /usr/bin/opt
RUN ln -s /usr/bin/llc-19 /usr/bin/llc

ENV CC=clang-19
ENV CXX=clang++-19
ENV OMPI_CC=$CC
ENV OMPI_CXX=$CXX

ENV OMPI_ALLOW_RUN_AS_ROOT=1
ENV OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1
ENV PMIX_MCA_gds="hash"
ENV OMPI_MCA_memory="^patcher"

# Install Python dependencies and ensure to activate virtualenv (by setting PATH variable)
ENV VIRTUAL_ENV=/opt/venv
RUN python3 -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"
RUN pip install http://apps.fz-juelich.de/jsc/jube/download.php?version=latest


# Compile CoVer
COPY ./CoVer /tmp/cover_src
WORKDIR /tmp/cover_src
RUN mkdir build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=/opt/cover -DCMAKE_BUILD_TYPE=Release && cmake --build . --target install -j

# Copy Bench files
COPY LULESH /root/LULESH
COPY jube_performance.xml /root

ENV PATH="/opt/cover/bin:$PATH"

WORKDIR /root
