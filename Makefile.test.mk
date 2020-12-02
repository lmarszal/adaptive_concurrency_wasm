.PHONY: setup_test build_test test clean_test

RUN = docker run --rm -v $(shell pwd):/work -w /work adaptive-concurrency-wasm-test-container bash -c

setup_test:
	docker build -t adaptive-concurrency-wasm-test-container -f hack/test.Dockerfile hack
	$(RUN) /work/hack/cmake.sh

build_test: setup_test
	$(RUN) "cd build && make"

test: setup_test build_test
	# TODO

clean_test:
	$(RUN) "cd build && make clean"
