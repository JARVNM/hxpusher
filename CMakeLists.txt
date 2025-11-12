cmake_minimum_required(VERSION 3.20)
project(hxpusher VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPOET_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOMOC ON)

set(QT_VERSION Qt6)
find_package(${QT_VERSION} REQUIRED COMPONENTS Core Widgets)

include_directories(.)

file (
    GLOB_RECURSE LOG_SOURCE
    CONFIGURE_DEPENDS
    "log/*.cc"
    "log/*.hh"
)

file(
    GLOB_RECURSE PROJECT_SOURCE
    CONFIGURE_DEPENDS
    "./*.cc"
    "./*.hh"
)

add_executable(
    ${PROJECT_NAME}
    ${PROJECT_SOURCE}
    ${LOG_SOURCE}
)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    ${QT_VERSION}::Widgets
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    librtmp.a
)