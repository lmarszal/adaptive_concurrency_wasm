cmake_minimum_required(VERSION 3.10)

project(adaptive-concurrency-wasm)

add_subdirectory(source)

enable_testing()
add_subdirectory(tests)
