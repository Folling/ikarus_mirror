cmake_minimum_required(VERSION 3.18)
project(ikarus)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

add_subdirectory(vendor)
add_subdirectory(include)
add_subdirectory(src)

add_library(
    ikarus
    ${INCLUDE_FILES}
    ${IMPL_HEADER_FILES}
    ${GENERATED_SOURCE_FILES}
    ${SOURCE_FILES}
)

add_custom_target(
    libikarus_gen
    VERBATIM
    DEPENDS ${GEN_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/gen/script/
    COMMAND cargo run --
    --input ${CMAKE_CURRENT_LIST_DIR}/vendor/ikarusdef/templates
    --include-output ${CMAKE_CURRENT_LIST_DIR}/include/ikarus
    --source-output ${CMAKE_CURRENT_LIST_DIR}/src/generated
    --impl-header-output ${CMAKE_CURRENT_LIST_DIR}/src/generated
)

add_dependencies(ikarus libikarus_gen)

target_include_directories(
    ikarus PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_include_directories(
    ikarus PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(
    ikarus PRIVATE
    gmp
    cppbase
    sqlitecpp
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
    cppbase
    sqlite
)
