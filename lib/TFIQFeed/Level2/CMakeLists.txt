# trade-frame/lib/TFIQFeed/Level2
cmake_minimum_required (VERSION 3.13)

PROJECT(TFIQFeedLevel2)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    Dispatcher.h
    FeatureSet.hpp
    FeatureSet_Level.hpp
    FeatureSet_Level_impl.hpp
    MsgOrderArrival.h
    MsgOrderDelete.h
    MsgPriceLevelArrival.h
    MsgPriceLevelDelete.h
    Symbols.hpp
  )

set(
  file_cpp
    Dispatcher.cpp
    FeatureSet.cpp
    FeatureSet_Level.cpp
    FeatureSet_Level_impl.cpp
    MsgOrderArrival.cpp
    MsgOrderDelete.cpp
    MsgPriceLevelArrival.cpp
    MsgPriceLevelDelete.cpp
    Symbols.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    "../.."
  )

