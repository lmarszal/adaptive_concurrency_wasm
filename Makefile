.PHONY:
all:
	# TODO download envoy sdk
	# TODO download https://github.com/nlohmann/json/releases/download/v3.9.1/json.hpp
	mkdir -p .wasm-cache
	docker build -t wasmsdk:v2 -f envoy/api/wasm/cpp/Dockerfile-sdk envoy/api/wasm/cpp
	docker run -v $(shell pwd)/source:/work -v ${shell pwd}/.wasm-cache:/root/emsdk/upstream/emscripten/cache -w /work  wasmsdk:v2 /build_wasm.sh
	./hack/create_configmap.sh > example/configmap.yaml source/adaptive_concurrency_filter.wasm

.PHONY:
clean:
	rm -fv source/*.wasm