project(GameFramework)

find_package(nlohmann_json CONFIG REQUIRED)

add_library(GameFramework STATIC

    Network/Service.cpp
    Network/Session.cpp
    Network/Listener.cpp
    Thread/ThreadPool.cpp
    Timer/AsioTimerManager.cpp
    Security/BasicXORCipher.cpp
    Dispatcher/DefaultDispatcher.h
    Dispatcher/IDispatcher.h
    Network/RecvBuffer.cpp
    Network/SessionManager.cpp
    Config/JsonConfigLoader.cpp
    # Add other sources as they are implemented
)

target_include_directories(GameFramework PUBLIC 
    .
)

target_link_libraries(GameFramework PUBLIC asio::asio nlohmann_json::nlohmann_json)

if(MSVC)
    target_compile_options(GameFramework PRIVATE /W4 /WX /utf-8)
    target_compile_definitions(GameFramework PRIVATE _WIN32_WINNT=0x0601)
else()
    target_compile_options(GameFramework PRIVATE -Wall -Wextra -Werror)
endif()

target_link_libraries(GameFramework PUBLIC ws2_32 mswsock)



