cmake_minimum_required(VERSION 3.10)
project(ChatWinApi)

set(CMAKE_CXX_STANDARD 17)

include_directories(include logics)

file(GLOB_RECURSE SOURCES
    "*.cpp"
    "logics/*.cpp"
    "src/*.cpp"
)

add_executable(ChatWinApi ${SOURCES})

if(WIN32)
    target_link_libraries(ChatWinApi ws2_32)
endif()