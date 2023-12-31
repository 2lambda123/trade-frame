# trade-frame/lib/TFVuTrading/MarketDepth
cmake_minimum_required (VERSION 3.13)

PROJECT(MarketDepth)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    Colours.hpp
    DataRowElement.hpp
    ExecutionControl.hpp
    Fields.hpp
    PanelLevelIIButtons.hpp
    PanelSideBySide.hpp
    PanelTrade.hpp
    PriceLevelOrder.hpp
    PriceRow.hpp
    PriceRows.hpp
    WinRow.hpp
    WinRowElement.hpp
  )

set(
  file_cpp
    DataRowElement.cpp
    ExecutionControl.cpp
    Fields.cpp
    PanelLevelIIButtons.cpp
    PanelSideBySide.cpp
    PanelTrade.cpp
    PriceLevelOrder.cpp
    PriceRow.cpp
    PriceRows.cpp
    WinRow.cpp
    WinRowElement.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )
# need to figure out how to make this work
#add_compile_options(`/usr/local/bin/wx-config --cxxflags`)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_FILE_OFFSET_BITS=64 )
target_compile_definitions(${PROJECT_NAME} PUBLIC -DWXUSINGDLL )
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__WXGTK__ )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
    "../.."
    "/usr/local/lib/wx/include/gtk3-unicode-${WX_VERSION_MAJOR}"
    "/usr/local/include/wx-${WX_VERSION_MAJOR}"
  )

