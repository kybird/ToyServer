project(GameClientApp)

add_executable(GameClient Main.cpp)

target_link_libraries(GameClient PRIVATE 
    GameFramework
    GameShared
    GameProtobuf
)
