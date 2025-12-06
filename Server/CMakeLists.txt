project(GameServerApp)

# Find tcmalloc from gperftools (vcpkg)
find_library(TCMALLOC_LIB tcmalloc_minimal PATHS "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/lib" REQUIRED)

add_executable(GameServer 
    Main.cpp
)

# resolve UTF-8 Hell!! BOM is evil!
target_compile_options(GameServer PRIVATE /utf-8)

target_link_libraries(GameServer PRIVATE 
    GameFramework
    GameShared
    GameDatabase
    GameProtobuf
    ${TCMALLOC_LIB}
)
