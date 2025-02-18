cmake_minimum_required(VERSION 3.29)
project(snake)
set(CMAKE_CXX_STANDARD 23)

find_package(SQLite3 REQUIRED)

add_executable(snake main.cpp
        desk/desk.h
        desk/desk.cpp
        desk/cell.h
        desk/cell.cpp
        database/database.h
        database/database.cpp)

target_link_libraries(snake PRIVATE SQLite3)