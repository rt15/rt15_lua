cmake_minimum_required(VERSION 3.13.4)

project(rt15_lua)

set(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ansi -pedantic -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-long-long")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finput-charset=cp1252")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mno-stack-arg-probe -fno-stack-check -fno-stack-protector")

string(REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

if(WIN32)
        add_compile_definitions(WIN32)
        add_compile_definitions(_CONSOLE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(BINARY_SUFFIX "d")
        add_compile_definitions(_DEBUG)
else()
        set(BINARY_SUFFIX "")
        add_compile_definitions(NDEBUG)
        add_link_options("-s")
endif()

include_directories(include)

include_directories(${PARENT_DIR}/rt15_portable_runtime/rpr/include)

if(WIN32)
        include_directories(${PARENT_DIR}/libs/lua/include)
endif()

link_directories(${PARENT_DIR}/rt15_portable_runtime/rpr/lib)

if(WIN32)
        link_directories(${PARENT_DIR}/libs/lua)
endif()

link_libraries(rpr${BINARY_SUFFIX})

if(WIN32)
        link_libraries(lua54)
endif()

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_library(${PROJECT_NAME}${BINARY_SUFFIX} SHARED ${SOURCES})

# Remove the lib prefix from the dll name.
if(WIN32)
        set_property(TARGET ${PROJECT_NAME}${BINARY_SUFFIX} PROPERTY PREFIX "")
endif()
