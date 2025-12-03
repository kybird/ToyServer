project(GameServerCore)

add_library(GameServerCore
    Network/Service.cpp
    Network/Session.cpp
    Network/Listener.cpp
    # Add other sources as they are implemented
)

target_include_directories(GameServerCore PUBLIC 
    include
    ${asio_SOURCE_DIR}/asio/include
)

if(MSVC)
    target_compile_options(GameServerCore PRIVATE /W4 /WX)
    target_compile_definitions(GameServerCore PRIVATE _WIN32_WINNT=0x0601)
else()
    target_compile_options(GameServerCore PRIVATE -Wall -Wextra -Werror)
endif()

target_link_libraries(GameServerCore PUBLIC ws2_32 mswsock)

# --- Redis (redis-plus-plus & hiredis) ---
include(FetchContent)

FetchContent_Declare(
    hiredis
    GIT_REPOSITORY https://github.com/redis/hiredis.git
    GIT_TAG master
    PATCH_COMMAND powershell -Command "(Get-Content CMakeLists.txt) -replace 'VERSION 3.0', 'VERSION 3.10' | Set-Content CMakeLists.txt"
)
FetchContent_MakeAvailable(hiredis)

FetchContent_Declare(
    redis-plus-plus
    GIT_REPOSITORY https://github.com/sewenew/redis-plus-plus.git
    GIT_TAG master
    PATCH_COMMAND powershell -Command "(Get-Content CMakeLists.txt) -replace 'VERSION 3.5', 'VERSION 3.10' | Set-Content CMakeLists.txt"
)
set(REDIS_PLUS_PLUS_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(REDIS_PLUS_PLUS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(redis-plus-plus)

target_link_libraries(GameServerCore PUBLIC redis++::redis++)
target_compile_definitions(GameServerCore PUBLIC HAS_REDIS)

# --- MySQL ---
# Assumes MySQL Connector/C or C++ is installed and accessible via CMAKE_PREFIX_PATH or standard paths
find_package(MySQL QUIET)
if (MySQL_FOUND)
    target_link_libraries(GameServerCore PUBLIC MySQL::MySQL)
    target_compile_definitions(GameServerCore PUBLIC HAS_MYSQL)
    message(STATUS "MySQL Found!")
else()
    message(WARNING "MySQL not found. Please install MySQL Connector/C++.")
endif()

# --- MongoDB ---
# Assumes mongo-cxx-driver is installed
find_package(mongocxx QUIET)
if (mongocxx_FOUND)
    target_link_libraries(GameServerCore PUBLIC mongo::mongocxx_shared)
    target_compile_definitions(GameServerCore PUBLIC HAS_MONGO)
    message(STATUS "MongoDB Found!")
else()
    message(WARNING "MongoDB not found. Please install mongo-cxx-driver.")
endif()
