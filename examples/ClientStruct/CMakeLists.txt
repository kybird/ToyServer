project(GameClientStruct)

add_executable(GameClient_Struct 
    Main.cpp
    GameClient.cpp
    ServerSession.cpp
    ClientPacketHandler.cpp
)

target_link_libraries(GameClient_Struct PRIVATE 
    GameFramework
    GameShared
    GameProtobuf
)
