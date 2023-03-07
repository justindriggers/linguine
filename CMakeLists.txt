cmake_minimum_required(VERSION 3.21)
project(linguine)

set(CMAKE_CXX_STANDARD 17)

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third_party")
set(AUDIO_DIR "${CMAKE_SOURCE_DIR}/audio")
set(RENDERERS_DIR "${CMAKE_SOURCE_DIR}/renderers")

add_compile_definitions(GLM_FORCE_LEFT_HANDED)

if (APPLE)
    add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)

    if (IOS)
        add_subdirectory(scampi)
    else()
        add_subdirectory(alfredo)
    endif()

    add_subdirectory("${AUDIO_DIR}/audioengine")
    add_subdirectory("${RENDERERS_DIR}/metal")
endif()

add_subdirectory(linguine)
