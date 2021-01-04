#load("@rules_proto//proto:defs.bzl", "proto_library")
load("//bazel/wasm:wasm.bzl", "wasm_cc_binary")

# proto_library(
#     name = "filter_proto",
#     srcs = [
#         "filter.proto",
#     ],
# )

# cc_proto_library(
#     name = "filter_cc_proto",
#     deps = [":filter_proto"],
# )

wasm_cc_binary(
    name = "adaptive_concurrency.wasm",
    srcs = glob([
        "src/*.cc",
        "src/*.hpp",
    ]),
    includes = [
        "src"
    ],
    deps = [
        #":filter_cc_proto",
        "@nlohmann_json//:json",
        "@envoy_wasm_api//:proxy_wasm_intrinsics",
        "//lib:adaptive_concurrency"
    ]
)
