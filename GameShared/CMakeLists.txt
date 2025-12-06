project(GameShared)

find_package(Protobuf REQUIRED)

# Generate Protobuf sources
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS Proto/Game.proto)

add_library(GameShared STATIC
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

target_include_directories(GameShared PUBLIC 
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(GameShared PUBLIC GameFramework protobuf::libprotobuf)

if(MSVC)
    target_compile_options(GameShared PRIVATE /W4 /utf-8)
else()
    target_compile_options(GameShared PRIVATE -Wall -Wextra)
endif()
