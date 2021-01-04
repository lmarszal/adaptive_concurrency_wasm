FROM ubuntu:bionic
RUN apt-get update && apt-get install -y --no-install-recommends curl ca-certificates
RUN curl https://github.com/bazelbuild/bazelisk/releases/download/v1.7.4/bazelisk-linux-amd64 -Lo bazelisk && chmod 755 bazelisk && mv bazelisk /usr/local/bin
RUN USE_BAZEL_VERSION=2.0.0 bazelisk version
RUN apt-get install -y --no-install-recommends git gcc-7 g++-7 cpp-7 python libxml2
ENV CC=gcc-7
ENV CXX=g++-7
ENV CPP=cpp-7
RUN curl -sL https://deb.nodesource.com/setup_12.x | bash -
RUN apt-get install -y --no-install-recommends nodejs