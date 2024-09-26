
cmake_minimum_required(VERSION 3.6)

project(talkApp VERSION 0.1 LANGUAGES C CXX)
add_executable(talkApp src/main.cpp src/server.cpp src/client.cpp src/ext_functions.cpp)

include(CTest)
enable_testing()

target_link_libraries(talkApp "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.26100.0/um/x64/Ws2_32.lib")

set(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++ -static")

set_property(TARGET talkApp PROPERTY CXX_STANDARD 17)

