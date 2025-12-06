add_executable(EchoServer EchoServer.cpp)
target_link_libraries(EchoServer PRIVATE GameFramework)
target_include_directories(EchoServer PRIVATE ${CMAKE_SOURCE_DIR}/Framework)
