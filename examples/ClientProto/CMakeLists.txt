project(GameClientProto)

add_executable(GameClient_Proto
    Main.cpp
    GameClient.cpp
    ServerSession.cpp
    ClientPacketHandler.cpp
)

target_link_libraries(GameClient_Proto PRIVATE 
    GameFramework
    GameShared
    GameProtobuf
)
