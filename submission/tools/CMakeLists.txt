cmake_minimum_required(VERSION 3.16)
project(scapms_submission LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(SQLite3 REQUIRED)

file(GLOB APP_SOURCES ${PROJECT_ROOT}/model/*.cpp ${PROJECT_ROOT}/controller/*.cpp ${PROJECT_ROOT}/utils/*.cpp ${PROJECT_ROOT}/gui/*.cpp)
file(GLOB APP_HEADERS ${PROJECT_ROOT}/gui/*.h)
file(GLOB TOOL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(build_submission ${TOOL_SOURCES} ${APP_SOURCES} ${APP_HEADERS})
target_include_directories(build_submission PRIVATE ${PROJECT_ROOT} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(build_submission PRIVATE Qt6::Widgets SQLite::SQLite3)
