FROM ubuntu:23.10

RUN apt-get update \
    && apt-get install -y --no-install-recommends python3-pip=23.1.2+dfsg-2 \
                                                  python3-venv=3.11.2-1 \
                                                  python-is-python3=3.11.4-1 \
                                                  cmake=3.25.1-1ubuntu2 \
    && rm -rf /var/lib/apt/lists/*

COPY . /app
WORKDIR /app

# hadolint ignore=SC1091
RUN python3 -m venv $(pwd) \
    && source $(pwd)/bin/activate \
    && pip install --no-cache-dir poetry==1.5.1 \
    && poetry env use system \
    && poetry install --only main

RUN make conan-profile && make build
