project(GameServerApp)

add_executable(GameServer Main.cpp)

target_link_libraries(GameServer PRIVATE GameServerCore)
