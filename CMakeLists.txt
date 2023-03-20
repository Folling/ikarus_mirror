cmake_minimum_required(VERSION 3.18)
project(ikarus)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

add_subdirectory(vendor)
add_subdirectory(gen)
add_subdirectory(include)
add_subdirectory(src)

add_library(
    ikarus SHARED
    ${INCLUDE_FILES}
    ${SOURCE_FILES}
)

add_custom_target(
    libikarus_gen
    COMMAND ${CMAKE_CURRENT_LIST_DIR}/gen/gen.rb ${CMAKE_CURRENT_LIST_DIR}/gen ${CMAKE_CURRENT_LIST_DIR}/include
)

add_dependencies(ikarus libikarus_gen)

target_include_directories(
    ikarus PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/src
)

find_package(ICU COMPONENTS uc i18n REQUIRED)

target_link_libraries(
    ikarus
    PRIVATE
    sqlite3
    fmt
    ${ICU_LIBRARIES}
)

set_target_properties(
    ikarus PROPERTIES
    LINKER_LANGUAGE C
)

add_executable(ikarus-tests test/main.cpp)

set_target_properties(
    ikarus-tests PROPERTIES
    LINKER_LANGUAGE CXX
)

target_link_libraries(
    ikarus-tests PRIVATE
    ikarus
    fmt
)

target_include_directories(
    ikarus-tests PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/src
)
