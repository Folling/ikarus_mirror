cmake_minimum_required(VERSION 3.18)
project(ikarus)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

add_subdirectory(vendor)
add_subdirectory(include)
add_subdirectory(src)

add_library(
    libikarus OBJECT
    ${INCLUDE_FILES}
    ${SOURCE_FILES}
)

target_include_directories(
    libikarus PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_include_directories(
    libikarus PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(
    libikarus PUBLIC
    Catch2::Catch2WithMain
)

target_link_libraries(
    libikarus PRIVATE
    cppbase
    sqlitecpp
)

set_target_properties(
    libikarus PROPERTIES
    LINKER_LANGUAGE CXX
)

add_executable(ikarus_tests ${SOURCE_FILES})
target_link_libraries(ikarus_tests PRIVATE Catch2::Catch2WithMain)

target_include_directories(
    ikarus_tests PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

include(CTest)
include(vendor/catch2/extras/Catch.cmake)
catch_discover_tests(ikarus_tests)
