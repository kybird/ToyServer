add_executable(EchoClient EchoClient.cpp)
target_link_libraries(EchoClient PRIVATE GameFramework)
target_include_directories(EchoClient PRIVATE ${CMAKE_SOURCE_DIR}/Framework)
