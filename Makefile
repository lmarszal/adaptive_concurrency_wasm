TOP := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

all: build

build: setup build_wasm
	./hack/create_configmap.sh extensions/adaptive_concurrency/plugin.wasm > example/adaptive_concurrency_configmap.yaml
	./hack/create_configmap.sh extensions/static_concurrency/plugin.wasm > example/static_concurrency_configmap.yaml

setup:
	# TODO download envoy sdk
	# TODO download https://github.com/nlohmann/json/releases/download/v3.9.1/json.hpp	
	mkdir -p .wasm-cache
	docker build -t wasmsdk:v2 -f envoy/api/wasm/cpp/Dockerfile-sdk envoy/api/wasm/cpp

build_wasm:
	$(foreach file, $(wildcard extensions/**/build_wasm.sh), cd $(TOP)/$(shell dirname $(file)) && bash ./build_wasm.sh &&) true

clean:
	rm -fv $(wildcard extensions/**/*.wasm)

.PHONY: all build clean