/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include <TFTrading/MacroStrand.h>

#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include "SimulationSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// sDirectory needs to be available on instantiation to enable signal availability
SimulationSymbol::SimulationSymbol(
  const std::string &sSymbol,
  pInstrument_cref pInstrument,
  const std::string &sGroup
  )
: Symbol<SimulationSymbol>(pInstrument), m_sDirectory( sGroup )
{
  // this is dealt with in the SimulationProvider, but we don't have a .Remove
  // m_OnTrade.Add( MakeDelegate( &m_simExec, &CSimulateOrderExecution::NewTrade ) );
}

SimulationSymbol::~SimulationSymbol() {
  // we don't yet have a .Remove for this in SimulationProvider.
  // m_OnTrade.Remove( MakeDelegate( &m_simExec, &CSimulateOrderExecution::NewTrade ) );

}

void SimulationSymbol::StartTradeWatch() {
  if ( 0 == m_trades.Size() ) {
    try {
      std::string sPath( m_sDirectory + Trades::Directory() + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO );
      HDF5TimeSeriesContainer<Trade> tradeRepository( dm, sPath );
      HDF5TimeSeriesContainer<Trade>::iterator begin, end;
      begin = tradeRepository.begin();
      end = tradeRepository.end();
      m_trades.Resize( end - begin );
      tradeRepository.Read( begin, end, &m_trades );
    }
    catch ( std::runtime_error &e ) {
      // couldn't do read, so leave as empty
    }
  }
}

void SimulationSymbol::StopTradeWatch() {
}

void SimulationSymbol::StartQuoteWatch() {
  if ( 0 == m_quotes.Size() ) {
    try {
      std::string sPath( m_sDirectory + Quotes::Directory() + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO );
      HDF5TimeSeriesContainer<Quote> quoteRepository( dm, sPath );
      HDF5TimeSeriesContainer<Quote>::iterator begin, end;
      begin = quoteRepository.begin();
      end = quoteRepository.end();
      m_quotes.Resize( end - begin );
      quoteRepository.Read( begin, end, &m_quotes );
    }
    catch ( std::runtime_error &e ) {
      // couldn't do read, so leave as empty
    }
  }
}

void SimulationSymbol::StopQuoteWatch() {
}

void SimulationSymbol::StartGreekWatch() {
  if ( ( 0 == m_greeks.Size() ) && ( m_pInstrument->IsOption() ) )  {
    try {
      std::string sPath( m_sDirectory + Greeks::Directory() + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO );
      HDF5TimeSeriesContainer<Greek> greekRepository( dm, sPath );
      HDF5TimeSeriesContainer<Greek>::iterator begin, end;
      begin = greekRepository.begin();
      end = greekRepository.end();
      m_greeks.Resize( end - begin );
      greekRepository.Read( begin, end, &m_greeks );
    }
    catch ( std::runtime_error &e ) {
      // couldn't do read, so leave as empty
    }
  }
}

void SimulationSymbol::StopGreekWatch() {
}

void SimulationSymbol::StartDepthWatch() {
  if ( 0 == m_depths.Size() )  {
    try {
      std::string sPath( m_sDirectory + DepthsByMM::Directory() + GetId() );
      ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO );
      HDF5TimeSeriesContainer<DepthByMM> depthRepository( dm, sPath );
      HDF5TimeSeriesContainer<DepthByMM>::iterator begin, end;
      begin = depthRepository.begin();
      end = depthRepository.end();
      m_depths.Resize( end - begin );
      depthRepository.Read( begin, end, &m_depths );
    }
    catch ( std::runtime_error &e ) {
      // couldn't do read, so leave as empty
    }
  }
}

void SimulationSymbol::StopDepthWatch() {
}

void SimulationSymbol::HandleQuoteEvent( const DatedDatum &datum ) {
  const Quote& quote( dynamic_cast<const Quote &>( datum ) );
  STRAND_CAPTURE( (m_OnQuote( quote )), quote )
}

void SimulationSymbol::HandleDepthEvent( const DatedDatum &datum ) {
  const DepthByMM& md( dynamic_cast<const DepthByMM &>( datum ) );
  STRAND_CAPTURE( (m_OnDepth( md )), md )
}

void SimulationSymbol::HandleTradeEvent( const DatedDatum &datum ) {
  const Trade& trade( dynamic_cast<const Trade &>( datum ) );
  STRAND_CAPTURE( (m_OnTrade( trade )), trade )
}

void SimulationSymbol::HandleGreekEvent( const DatedDatum &datum ) {
  const Greek& greek( dynamic_cast<const Greek &>( datum ) );
  STRAND_CAPTURE( (m_OnGreek( greek )), greek )
}

} // namespace tf
} // namespace ou
