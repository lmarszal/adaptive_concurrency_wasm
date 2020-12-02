file(GLOB source_SRCS ./**.cc ./**.hpp)

add_library(ac STATIC
    ${source_SRCS}
)

target_include_directories(ac PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_property(TARGET ac PROPERTY CXX_STANDARD 17)
