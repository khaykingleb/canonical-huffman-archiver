FROM ubuntu:22.04

# Install basic dependencies
RUN apt-get update \
    && apt-get install --no-install-recommends -y build-essential=12.9ubuntu3 \
                                                  cmake=3.22.1-1ubuntu1.22.04.2 \
                                                  python3=3.10.6-1~22.04 \
                                                  python3-pip=22.0.2+dfsg-1ubuntu0.4 \
                                                  python-is-python3=3.9.2-2 \
                                                  wget=1.21.2-2ubuntu1 \
                                                  lsb-release=11.1.0ubuntu4 \
                                                  software-properties-common=0.99.22.9 \
                                                  gnupg=2.2.27-3ubuntu2.1 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Install LLVM
RUN wget https://apt.llvm.org/llvm.sh --quiet \
    && chmod u+x llvm.sh \
    && ./llvm.sh 18

# Setup clang as default compiler
RUN ln -s /usr/bin/clang-18 /usr/bin/clang
ENV CC=clang

RUN ln -s /usr/bin/clang++-18 /usr/bin/clang++
ENV CXX=clang++

# Install Conan package manager for C++
WORKDIR /app
COPY pyproject.toml poetry.lock ./
RUN --mount=type=cache,target=/root/.cache/pypoetry/cache \
    --mount=type=cache,target=/root/.cache/pypoetry/artifacts \
    pip install --no-cache-dir poetry==1.8.2 \
        && poetry config virtualenvs.create false \
        && poetry install --only main

# Copy the project files
COPY . ./
