project(GameProtobuf)

find_package(Protobuf REQUIRED)

add_library(GameProtobuf INTERFACE)

target_include_directories(GameProtobuf INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(GameProtobuf INTERFACE
    GameFramework
    protobuf::libprotobuf
)

if(MSVC)
    target_compile_options(GameProtobuf INTERFACE /W4 /utf-8)
else()
    target_compile_options(GameProtobuf INTERFACE -Wall -Wextra)
endif()
