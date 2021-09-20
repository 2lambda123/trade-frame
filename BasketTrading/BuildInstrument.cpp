/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

/*
 * File:    BuildInstrument.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created on Sept 20, 2021, 21:52
 */

#include <TFTrading/Watch.h>
#include <TFTrading/InstrumentManager.h>

#include <TFIQFeed/BuildInstrument.h>

#include "BuildInstrument.h"
#include "TFTrading/Watch.h"

using pWatch_t = ou::tf::Watch::pWatch_t;

BuildInstrument::BuildInstrument( pProviderIQFeed_t pIQFeed, pProviderIBTWS_t pIB, fGetTableRowDef_t&& fGetTableRowDef )
: m_pIQ( std::move( pIQFeed ) ), m_pIB( std::move( pIB ) ),
  m_fGetTableRowDef( std::move( fGetTableRowDef ) )
{
  assert( m_pIQ );
  assert( m_pIB );
  assert( m_fGetTableRowDef );
}

void BuildInstrument::Add( const std::string& sIQFeedSymbol, fInstrument_t&& fInstrument ) {

  {
    std::lock_guard<std::mutex> lock( m_mutexMap );
    setSymbol_t::const_iterator iter = m_setSymbolUnique.find( sIQFeedSymbol );
    if ( m_setSymbolUnique.end() == iter ) {
      m_setSymbolUnique.emplace( sIQFeedSymbol );
      m_mapSymbol.emplace( std::make_pair( sIQFeedSymbol, std::move( fInstrument ) ) );
    }
    else {
      std::cout << "BuildInstrument::Add: " << sIQFeedSymbol << " duplicate request" << std::endl;
    }
  }

  Update();

}

void BuildInstrument::Update() {

  bool bProceed( false );
  mapInProgress_t::iterator iterInProgress;

  {
    std::lock_guard<std::mutex> lock( m_mutexMap );

    if ( 5 > m_mapInProgress.size() ) { // 5 is set in m_pIB
      if ( 0 != m_mapSymbol.size() ) {

        mapSymbol_t::iterator iterSymbol = m_mapSymbol.begin();
        auto& [ sIQFeedSymbol_, fInstrument_ ] = *iterSymbol;
        const std::string sIQFeedSymbol( sIQFeedSymbol_ );

        auto result = m_mapInProgress.emplace( std::make_pair( sIQFeedSymbol, InProgress( std::move( fInstrument_ ) ) ) );
        assert( result.second );

        m_mapSymbol.erase( iterSymbol );

        iterInProgress = result.first;
        bProceed = true;

        //std::cout << "BuildInstrument::Update " << sSymbol << std::endl;
      }
    }
  }

  if ( bProceed ) Build( iterInProgress );

}

void BuildInstrument::Build( mapInProgress_t::iterator iterInProgress ) {

  auto& [ sIQFeedSymbol, ip ] = *iterInProgress;

  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() );

  pInstrument_t pInstrument;

  // TODO: will probably need to fix this call, as it has an exemption on already loaded symbols
  pInstrument = im.LoadInstrument( ou::tf::keytypes::EProviderIQF, sIQFeedSymbol );
  if ( pInstrument ) { // skip the build
    std::cout << "BuildInstrument::Build existing: " << pInstrument->GetInstrumentName() << std::endl;
    ip.fInstrument( pInstrument );

    std::lock_guard<std::mutex> lock( m_mutexMap );
    m_mapInProgress.erase( iterInProgress );
  }
  else { // bulid a new instrument

    const trd_t& trd( m_fGetTableRowDef( sIQFeedSymbol ) ); // TODO: check for errors

    pInstrument = ou::tf::iqfeed::BuildInstrument( "Acquire-" + sIQFeedSymbol, trd );
    pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pIQ );

    AcquireFundamentals::pAcquireFundamentals_t pAcquireFundamentals
      = std::make_shared<AcquireFundamentals>(
          std::move( pWatch ),
          [this,iterInProgress,&trd]( pWatch_t pWatchOld ) { // async call once fundamentals arrive

            const ou::tf::Watch::Fundamentals& fundamentals( pWatchOld->GetFundamentals() );
            const std::string sGenericName
              = ou::tf::iqfeed::MarketSymbol::BuildGenericName( fundamentals.sExchangeRoot, trd, fundamentals.dateExpiration );
            pInstrument_t pInstrument
              = ou::tf::iqfeed::BuildInstrument( sGenericName, trd, fundamentals );
            pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, pWatchOld->GetProvider() );

            std::cout
              << "MasterPortfolio::BuildInstrument build: "
              << iterInProgress->first << ","
              << sGenericName << ","
              << fundamentals.sExchangeRoot
              << std::endl;

            m_pIB->RequestContractDetails(
              fundamentals.sExchangeRoot,  // needs to be the IB base name
              pInstrument,  // this is a filled-in, prepared instrument
              [pWatch,iterInProgress]( const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument ){
                //std::cout << "BuildInstrument::Build contract: " << pInstrument->GetInstrumentName() << std::endl;
                assert( 0 != pInstrument->GetContract() );
                ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() );
                im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
                iterInProgress->second.fInstrument( pInstrument );
              },
              [this,iterInProgress](){
                // TODO: how to test for incomplete done?
                //std::cout << "BuildInstrument::Build done: " << iterInProgress->first << std::endl;
                {
                  std::lock_guard<std::mutex> lock( m_mutexMap );
                  m_mapInProgress.erase( iterInProgress );
                }
                Update();
              }
              );
            //std::cout << "BuildInstrument::Build begin: " << iterInProgress->first << std::endl;
          }
        );

    iterInProgress->second.pAcquireFundamentals = pAcquireFundamentals;
    pAcquireFundamentals->Start();
  }
}

void BuildInstrument::Clear() {
  std::lock_guard<std::mutex> lock( m_mutexMap );
  assert( 0 == m_mapInProgress.size() );
  m_mapSymbol.clear();
}

