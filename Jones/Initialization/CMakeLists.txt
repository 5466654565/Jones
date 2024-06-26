cmake_minimum_required(VERSION 3.20)
project(Initialization)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

find_package(mlpack REQUIRED)
find_package(SQLite3 REQUIRED)

#set(BRIDGE_PATH ".../Jones_C++/Bridge")
include_directories("${BRIDGE_PATH}")
link_directories("${BRIDGE_PATH}/x64/Debug")

add_executable(Initialization main.cpp)

add_custom_command(TARGET Initialization POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #".../Jones_C++/Bridge/x64/Debug/Bridge.dll"
    $<TARGET_FILE_DIR:Initialization>)

target_link_libraries(Initialization PRIVATE
    mlpack::mlpack
    SQLite3::SQLite3
    ${BRIDGE_PATH}/x64/Debug/Bridge
)