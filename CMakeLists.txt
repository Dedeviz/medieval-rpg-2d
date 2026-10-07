cmake_minimum_required(VERSION 3.16)
project(1223_Italian_Kingdoms LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(ItalianKingdoms src/main.cpp)
target_link_libraries(ItalianKingdoms PRIVATE sfml-graphics sfml-window sfml-system)
