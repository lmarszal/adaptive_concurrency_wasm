.PHONY: all build clean

TOP := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

all: build

build: build_wasm
	

include Makefile.wasm.mk

clean:
	rm -fv $(wildcard extensions/**/*.wasm)
