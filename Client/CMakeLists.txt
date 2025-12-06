project(GameClientApp)

add_executable(GameClient 
    Main.cpp
    GameClient.cpp
    ServerSession.cpp
    ClientPacketHandler.cpp
)

target_link_libraries(GameClient PRIVATE 
    GameFramework
    GameShared
    GameProtobuf
)
