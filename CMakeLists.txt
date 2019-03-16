project( Simple )
cmake_minimum_required(VERSION 2.8.10)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
IF( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
    EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
        )
ENDIF()

option(BUILD_SANITIZED "Build libraries and test with memory sanitizer enabled" OFF)
if(BUILD_SANITIZED)
    message(STATUS "Enabling memory sanitizer")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif(BUILD_SANITIZED)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS)

find_package(Threads REQUIRED)
add_library(Simple INTERFACE)
target_include_directories(Simple INTERFACE "include/")
target_link_libraries(Simple INTERFACE ${CONAN_LIBS} Threads::Threads)
target_compile_options(Simple INTERFACE "-Wall" "-Wextra")

# add test targets
option(BUILD_TESTS "Build tests of simple library" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
