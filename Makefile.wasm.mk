.PHONY: setup_wasm build_wasm clean_wasm

TOP := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

build_wasm: setup_wasm
	$(foreach file, $(wildcard extensions/**/build_wasm.sh), cd $(TOP)/$(shell dirname $(file)) && bash ./build_wasm.sh &&) true
	./hack/create_configmap.sh adaptive_concurrency_filter.wasm extensions/adaptive_concurrency/plugin.wasm > example/adaptive_concurrency_configmap.yaml
	#./hack/create_configmap.sh static_concurrency_filter.wasm extensions/static_concurrency/plugin.wasm > example/static_concurrency_configmap.yaml

setup_wasm: envoy
	mkdir -p .wasm-cache
	docker build -t wasmsdk:v2 -f envoy/api/wasm/cpp/Dockerfile-sdk envoy/api/wasm/cpp

# TODO download SDK instead of full envoy
ENVOY_SHA=56b2f1495e121ab86e6de1497b3287023378bfc1

envoy:
	curl -L https://github.com/istio/envoy/archive/${ENVOY_SHA}.tar.gz | tar xz
	mv envoy-${ENVOY_SHA} envoy

clean_wasm:
	rm -fv $(wildcard extensions/**/*.wasm)
