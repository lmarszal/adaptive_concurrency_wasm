load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "adaptive_concurrency",
    srcs = glob([
        "src/*.cc",
        "src/*.hpp"
    ]),
    includes = [
        "src"
    ],
    hdrs = glob([
        "src/*.hpp",
    ]),
    visibility = ["//visibility:public"],
)

cc_test(
    name = "test",
    srcs = glob([
        "test/*.cc"
    ]),
    deps = [
        "@gtest//:gtest",
        "@gtest//:gtest_main",
        ":adaptive_concurrency"
    ]
)
