.PHONY: all build clean

all: test build

build: build_wasm

include Makefile.wasm.mk
include Makefile.test.mk

clean: clean_wasm clean_test
