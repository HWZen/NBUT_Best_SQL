cmake_minimum_required(VERSION 3.16)
project(NBUT_Best_SQL)

set(CMAKE_CXX_STANDARD 14)

include_directories(Inc)

file(GLOB_RECURSE server_src "Server_Src/*.cpp")
file(GLOB_RECURSE client_src "Client_Src/*.cpp")

find_package(Threads)
add_executable(server ${server_src} Inc/function.cpp)
target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT})

add_executable(client ${client_src} Inc/function.cpp)
#add_executable(temp temp.cpp)
#add_executable(temp2 Server_Src/sock5_server_temp.cpp)