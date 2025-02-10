cmake_minimum_required(VERSION 3.16)
project(KakatuaEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(KakatuaEditor main.cpp)
target_link_libraries(KakatuaEditor Qt6::Widgets)