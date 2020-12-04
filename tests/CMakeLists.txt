cmake_minimum_required(VERSION 3.10)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# google test
include(ExternalProject)
ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/release-1.10.0.tar.gz
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gtest source_dir binary_dir)
add_library(libgtest INTERFACE)
add_dependencies(libgtest gtest)
target_link_libraries(libgtest
    INTERFACE Threads::Threads
    "${binary_dir}/lib/libgtest_main.a"
    "${binary_dir}/lib/libgtest.a")
include_directories("${source_dir}/googletest/include")

# tests
file(GLOB test_cases *.cc)
foreach(case_file ${test_cases})
    get_filename_component( case_name ${case_file} NAME_WE )
    set (case_name test_${case_name})
    add_executable(${case_name} ${case_file})
    target_link_libraries(${case_name} libgtest ac)
    set_property(TARGET ${case_name} PROPERTY CXX_STANDARD 17)
    add_test(NAME ${case_name}
             COMMAND ${PROJECT_BINARY_DIR}/tests/${case_name}
             WORKING_DIRECTORY
             ${PROJECT_BINARY_DIR}
             )
endforeach()
