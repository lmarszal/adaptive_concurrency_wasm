CPP_API=/sdk
CPP_CONTEXT_LIB = ${CPP_API}/proxy_wasm_intrinsics.cc
ABSL = /root/abseil-cpp
# TODO switch to bazel and use absl mutex
#ABSL_CPP = ${ABSL}/absl/synchronization/*.a ${ABSL}/absl/base/*.a ${ABSL}/absl/debugging/*.a ${ABSL}/absl/time/*.a

all: static_concurrency_filter.wasm adaptive_concurrency_filter.wasm

# TODO sompile common *.cc files

%.wasm %.wat: %.cc ${CPP_API}/proxy_wasm_intrinsics.h ${CPP_API}/proxy_wasm_enums.h ${CPP_API}/proxy_wasm_externs.h ${CPP_API}/proxy_wasm_api.h ${CPP_API}/proxy_wasm_intrinsics.js ${CPP_CONTEXT_LIB}
	em++ -s STANDALONE_WASM=1 -s EMIT_EMSCRIPTEN_METADATA=1 -s EXPORTED_FUNCTIONS=['_malloc'] \
		 --std=c++17 -O3 -flto -s WASM_OBJECT_FILES=0 --llvm-lto 1 -DPROXY_WASM_PROTOBUF_LITE=1 \
		 -I${CPP_API} -I/usr/local/include -I${ABSL} \
		 --js-library ${CPP_API}/proxy_wasm_intrinsics.js \
		 $*.cc ${CPP_API}/proxy_wasm_intrinsics_lite.pb.cc ${CPP_API}/struct_lite.pb.cc ${CPP_CONTEXT_LIB} ${CPP_API}/libprotobuf-lite.a \
		 -o $*.wasm
