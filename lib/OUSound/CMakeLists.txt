# trade-frame/lib/OUSound
cmake_minimum_required (VERSION 3.13)

PROJECT(OUSound)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    Music.hpp
    PortAudio.hpp
    WaveGen.hpp
  )

set(
  file_cpp
    Music.cpp
    PortAudio.cpp
    WaveGen.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
  )

