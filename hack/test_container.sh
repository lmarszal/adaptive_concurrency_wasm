#!/bin/bash

set -e

# basics
apt-get update
apt-get install -y --no-install-recommends ca-certificates curl

# gcc-7
apt-get install -y --no-install-recommends gcc-7 g++-7 cpp-7
export CC=gcc-7
export CXX=g++-7
export CPP=cpp-7

# cmake
apt-get install -y --no-install-recommends build-essential cmake
