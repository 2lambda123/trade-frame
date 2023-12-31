# trade-frame/lib/TFIQFeed
cmake_minimum_required (VERSION 3.13)

PROJECT(TFIQFeed)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    BarHistory.h
    BuildInstrument.h
    BuildSymbolName.h
    CurlGetMktSymbols.h
    HistoryRequest.h
    InMemoryMktSymbolList.h
    IQFeed.h
    HistoryBulkQuery.h
    HistoryBulkQueryMsgShim.h
#    HistoryCollector.h
    HistoryQuery.h
    HistoryQueryMsgShim.h
#    InstrumentFile.h
    Messages.h
    MsgShim.h
    NewsQuery.h
    NewsQueryMsgShim.h
    Provider.h
#    SymbolFile.h
    Symbol.h
    LoadMktSymbols.h
    MarketSymbol.h
    MarketSymbols.h
    OptionChainQuery.h
    Option.h
    ParseFOptionDescription.h
    ParseMktSymbolDiskFile.h
    ParseMktSymbolLine.h
    ParseOptionDescription.h
    ParseOptionSymbol.h
    SecurityType.h
    SymbolLookup.h
    UnzipMktSymbols.h
    ValidateMktSymbolLine.h
  )

set(
  file_cpp
    BarHistory.cpp
    BuildInstrument.cpp
    BuildSymbolName.cpp
    CurlGetMktSymbols.cpp
    HistoryRequest.cpp
    InMemoryMktSymbolList.cpp
    IQFeed.cpp
#    HistoryCollector.cpp
#    InstrumentFile.cpp
    Messages.cpp
    Provider.cpp
    Symbol.cpp
#    SymbolFile.cpp
    LoadMktSymbols.cpp
    MarketSymbol.cpp
    MarketSymbols.cpp
    OptionChainQuery.cpp
    Option.cpp
    ParseMktSymbolDiskFile.cpp
    ParseMktSymbolLine.cpp
    SymbolLookup.cpp
    UnzipMktSymbols.cpp
    ValidateMktSymbolLine.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${file_h}
  ${file_cpp}
  )

add_subdirectory(Level2)

# issue with boost 1.81.0, maybe fix when 1.81.1 or 1.82 is available
# https://github.com/boostorg/phoenix/issues/111
target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_PHOENIX_STL_TUPLE_H_)
target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
  )

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
    TFSimulation
  )
