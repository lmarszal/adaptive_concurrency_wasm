CPP_API=/sdk
CPP_CONTEXT_LIB = ${CPP_API}/proxy_wasm_intrinsics.cc

COMMON_HEADERS = $(wildcard extensions/common/*.hpp) $(wildcard source/*.hpp)
COMMON_SRCS = $(wildcard extensions/common/*.cc) $(wildcard source/*.cc)

all: plugin.wasm

%.wasm %.wat: %.cc ${CPP_API}/proxy_wasm_intrinsics.h ${CPP_API}/proxy_wasm_enums.h ${CPP_API}/proxy_wasm_externs.h ${CPP_API}/proxy_wasm_api.h ${CPP_API}/proxy_wasm_intrinsics.js ${CPP_CONTEXT_LIB} ${COMMON_HEADERS} ${COMMON_SRCS}
	em++ -s STANDALONE_WASM=1 -s EMIT_EMSCRIPTEN_METADATA=1 -s EXPORTED_FUNCTIONS=['_malloc','_free'] \
		 --std=c++17 -O3 -flto -s WASM_OBJECT_FILES=0 --llvm-lto 1  -DPROXY_WASM_PROTOBUF_LITE=1 \
		 -I${CPP_API} -I/usr/local/include -I. -I /work/extensions/common -I /work/source \
		 --js-library ${CPP_API}/proxy_wasm_intrinsics.js \
		 $*.cc ${CPP_API}/proxy_wasm_intrinsics_lite.pb.cc ${CPP_API}/struct_lite.pb.cc ${CPP_CONTEXT_LIB} ${COMMON_SRCS} ${CPP_API}/libprotobuf-lite.a \
		 -o $*.wasm
