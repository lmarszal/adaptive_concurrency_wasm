file(GLOB source_SRCS ./**.cc ./**.hpp)

add_library(acmock STATIC
    ${source_SRCS}
)

target_include_directories(acmock PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(acmock ac)

set_property(TARGET acmock PROPERTY CXX_STANDARD 17)
