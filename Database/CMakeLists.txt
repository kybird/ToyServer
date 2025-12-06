project(GameDatabase)

add_library(GameDatabase STATIC
    MySQLDatabase.cpp
    MongoDBDatabase.cpp
    RedisDatabase.cpp
)

target_include_directories(GameDatabase PUBLIC 
    .
)

target_link_libraries(GameDatabase PUBLIC 
    GameFramework
)

# --- Redis (redis-plus-plus & hiredis) ---
find_package(hiredis CONFIG REQUIRED)
find_package(redis++ CONFIG REQUIRED)
target_link_libraries(GameDatabase PUBLIC redis++::redis++)
target_compile_definitions(GameDatabase PUBLIC HAS_REDIS)

# --- MySQL ---
find_package(unofficial-libmysql CONFIG REQUIRED)
target_link_libraries(GameDatabase PUBLIC unofficial::libmysql::libmysql)
target_compile_definitions(GameDatabase PUBLIC HAS_MYSQL)

# --- MongoDB ---
find_package(utf8proc CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(mongocxx CONFIG REQUIRED)
target_link_libraries(GameDatabase PUBLIC mongo::mongocxx_shared)
target_compile_definitions(GameDatabase PUBLIC HAS_MONGO)

if(MSVC)
    target_compile_options(GameDatabase PRIVATE /W4 /WX /utf-8)
else()
    target_compile_options(GameDatabase PRIVATE -Wall -Wextra -Werror)
endif()
