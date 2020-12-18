cmake_minimum_required(VERSION 3.17)
project(NBUT_Best_SQL)

set(CMAKE_CXX_STANDARD 14)

include_directories(Inc)

file(GLOB_RECURSE server_src "Server_Src/*.cpp")
file(GLOB_RECURSE client_src "Client_Src/*.cpp")


add_executable(server ${server_src})
add_executable(client ${client_src})
add_executable(temp temp.cpp)
add_executable(temp2 temp.sock5_server.cpp)