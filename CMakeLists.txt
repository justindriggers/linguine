cmake_minimum_required(VERSION 3.21)
project(linguine)

set(CMAKE_CXX_STANDARD 17)

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third_party")

add_subdirectory(linguine)

if (APPLE)
    if (IOS)
        add_subdirectory(scampi)
    else()
        add_subdirectory(alfredo)
    endif()
endif()
