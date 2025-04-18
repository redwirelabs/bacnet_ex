cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

# setup bacnet-stack
set(bacnet-stack_DIR "../bacnet-stack/build")
link_directories(${bacnet-stack_DIR})
include_directories(../bacnet-stack/src)

option(USE_UDP_SOCKETS "Use UDP Sockets instead of Elixir's Port module" OFF)

# setup erlang
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} $ENV{ERL_EI_LIBDIR})
find_library(ERLANG_EI_LIB ei)
include_directories($ENV{ERL_EI_INCLUDE_DIR})

# sources
INCLUDE_DIRECTORIES(src/)
set(SOURCES
    src/bacnet.c
    src/log.c
    src/main.c
    src/port.c
    src/object/binary_input.c
    src/object/characterstring_value.c
    src/object/command.c
    src/protocol/decode_call.c
    src/protocol/enum.c
    src/protocol/event.c)

# If not built by Mix, it is necessary to set the variable to
# the current directory
if(DEFINED ENV{MIX_APP_PATH} AND NOT "$ENV{MIX_APP_PATH}" STREQUAL "")
    set(MIX_APP_PATH $ENV{MIX_APP_PATH})
else()
    set(MIX_APP_PATH ".")
endif()

# build
project(bacnetd)
# Release
# set(CMAKE_BUILD_TYPE Release)
# Debug
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
add_executable(bacnetd ${SOURCES})
set_target_properties(bacnetd PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${MIX_APP_PATH}/priv)

if(USE_UDP_SOCKETS)
    target_compile_definitions(bacnetd PRIVATE USE_UDP_SOCKETS)
endif()
# target_link_libraries(bacnetd PRIVATE ${ERLANG_EI_LIB})
target_link_libraries(bacnetd PRIVATE :libbacnet-stack.a ${ERLANG_EI_LIB})
