cmake_minimum_required (VERSION 2.8)
project(luaset)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
    set(CMAKE_MACOSX_RPATH 0)
endif()

if(WIN32)
	set(suffix ".dll")
    add_definitions(-DLUA_BUILD_AS_DLL -DLUA_LIB -DHTTP_PARSER_STRICT=0)
else()
	set(suffix ".so")
endif()

macro (add_lua_library lname)
    add_library(${lname} SHARED ${ARGN})
    set_target_properties(${lname} PROPERTIES PREFIX "")
    set_target_properties(${lname} PROPERTIES SUFFIX ${suffix})
endmacro()

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

set(LUA_DIR $ENV{HOME}/.local/lua53)

set(LUA_INCLUDE_DIR  ${LUA_DIR}/include)
include_directories (${LUA_INCLUDE_DIR})

set(LUA_LIB_DIR ${LUA_DIR}/lib)
link_directories(${LUA_LIB_DIR})


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(ch27)
include_directories(${PROJECT_SOURCE_DIR}/ch27)

add_subdirectory(ch28)
add_subdirectory(ch29)
add_subdirectory(ch31)
add_subdirectory(ch32)