cmake_minimum_required(VERSION 3.11)

include(FetchContent)

FetchContent_Declare(
  unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG        860062d51b2e8a75d150337b63ca2a472840d13c # v2.6.0
)

FetchContent_MakeAvailable(unity)

project(urdflib
    VERSION 0.1
    DESCRIPTION "RDF library for constrained devices"
    LANGUAGES C)

include_directories(include)
add_library(urdflib src/urdflib.c)
target_link_libraries(urdflib PUBLIC cbor)

add_executable(test test/test.c)
target_link_libraries(test PUBLIC urdflib)
target_link_libraries(test PUBLIC unity)

add_executable(coswot examples/unix/main.c)
target_link_libraries(coswot PUBLIC urdflib)
