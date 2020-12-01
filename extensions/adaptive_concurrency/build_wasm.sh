#!/bin/bash

set -e

docker run -e uid="$(id -u)" -e gid="$(id -g)" \
    -v $PWD:/work -w /work \
    -v $(realpath $PWD/../../extensions):/work/extensions \
    -v $PWD/../../.wasm-cache:/root/emsdk/upstream/emscripten/cache \
    wasmsdk:v2 bash /build_wasm.sh
rmdir extensions