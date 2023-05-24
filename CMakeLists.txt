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
    # TODO GEN_TARGET_FILES only lists the files in the include directory
    BYPRODUCTS ${GEN_TARGET_FILES}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/gen/script_old/
    COMMAND cargo run --
    --input ${PROJECT_SOURCE_DIR}/vendor/ikarusdef/templates
    --include-output ${PROJECT_SOURCE_DIR}/include/ikarus
    --source-output ${PROJECT_SOURCE_DIR}/src/generated
    --impl-header-output ${PROJECT_SOURCE_DIR}/src/generated
)

add_custom_target(
    libikarus_fmt
    VERBATIM
    USES_TERMINAL
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    # @formatter:off
    COMMAND find include/ src/ -type f (-iname *.hpp -o -iname *.cpp -o -iname *.h -o -iname *.c) -exec clang-format -i --verbose --style=file {} \;
    # @formatter:on
)

add_dependencies(libikarus_fmt libikarus_gen)

add_dependencies(ikarus libikarus_gen libikarus_fmt)

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
    cppbase
    sqlitecpp
    gmp
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
