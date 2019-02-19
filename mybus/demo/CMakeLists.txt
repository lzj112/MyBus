

cmake_minimum_required (VERSION 3.0)

project (mybus_test)


add_subdirectory(src)

add_definitions("-g -Wall -std=c++11")

add_executable(write1 testWrite.cpp)
target_link_libraries(write1 MyBus -lpthread)

add_executable(write2 testWrite2.cpp)
target_link_libraries(write2 MyBus -lpthread)

add_executable(read testRead.cpp)
target_link_libraries(read MyBus -lpthread)

add_executable(transit1 BusServer1.cpp)
target_link_libraries(transit1 MyBus -lpthread)

add_executable(transit2 BusServer2.cpp)
target_link_libraries(transit2 MyBus -lpthread)