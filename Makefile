OUTPUT_USER_ROOT=$(shell pwd)/.bazel
RUN = docker run --rm -v $(shell pwd):$(shell pwd) -w $(shell pwd) adaptive-concurrency-bazel-test-container bash -c

.PHONY: all setup build test clean

all: test build

setup:
	mkdir -p ./bazel
	docker build -t adaptive-concurrency-bazel-test-container -f hack/Dockerfile hack

build: setup
	$(RUN) "bazelisk --output_user_root=$(OUTPUT_USER_ROOT) build //plugin:adaptive_concurrency.wasm"
	$(RUN) "./hack/create_configmap.sh adaptive_concurrency_filter.wasm bazel-bin/plugin/adaptive_concurrency.wasm > example/adaptive_concurrency_configmap.yaml"

test: setup
	$(RUN) "bazelisk --output_user_root=$(OUTPUT_USER_ROOT) test //lib:test"

clean:
	$(RUN) "bazelisk clean"
	rm -rf .bazel
