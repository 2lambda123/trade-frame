/* Copyright(c) 2017, One Unified. All rights reserved.                 *
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
 * File:   IQFeedInstrumentBuild.cpp
 * Author: raymond@burkholder.net
 *
 * Created on August 19, 2017, 7:12 PM
 */

#include <functional>

#include <TFTrading/TradingEnumerations.h>

#include <TFIQFeed/BuildSymbolName.h>

#include "IQFeedInstrumentBuild.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

IQFeedInstrumentBuild::IQFeedInstrumentBuild( wxWindow* pParentForDialog ) {

  assert( 0 != pParentForDialog );
  m_pParentForDialog = pParentForDialog;

  m_pDialogPickSymbol = 0;

  namespace ph = std::placeholders;
  m_de.fLookupIQFeedDescription = std::bind( &IQFeedInstrumentBuild::HandleLookUpDescription, this, ph::_1, ph::_2 );
  m_de.fComposeIQFeedFullName = std::bind( &IQFeedInstrumentBuild::HandleComposeIQFeedFullName, this, ph::_1 );

}

IQFeedInstrumentBuild::~IQFeedInstrumentBuild() {
}

IQFeedInstrumentBuild::pInstrument_t
  IQFeedInstrumentBuild::HandleNewInstrumentRequest(
    const ou::tf::Allowed::EInstrument selector,
    const wxString& wxsUnderlying // optional
) {

  assert( 0 == m_pDialogPickSymbol );

  m_pDialogPickSymbolCreatedInstrument.reset();

  m_de.sIQFSymbolName = wxsUnderlying;

  m_pDialogPickSymbol = new ou::tf::DialogPickSymbol( m_pParentForDialog );
  m_pDialogPickSymbol->SetDataExchange( &m_de );

  switch ( selector ) {
    case ou::tf::Allowed::All:
      m_pDialogPickSymbol->SetAll();
      break;
    case ou::tf::Allowed::Basic:
      m_pDialogPickSymbol->SetBasic();
      break;
    case ou::tf::Allowed::FuturesOptions:
      m_pDialogPickSymbol->SetFuturesOptionOnly();
      break;
    case ou::tf::Allowed::Options:
      m_pDialogPickSymbol->SetOptionOnly();
      break;
    case ou::tf::Allowed::None:
      // should be no instrument popup
      assert( 0 );
      break;
  }

  int status = m_pDialogPickSymbol->ShowModal();

  //std::cout << "IQFeedInstrumentBuild::HandleNewInstrumentRequest status " << status << std::endl;
  switch ( status ) {
    case wxID_CANCEL:
      m_pDialogPickSymbolCreatedInstrument.reset();
      // menu item should be deleting
      break;
    case wxID_OK:
      if ( nullptr == m_pDialogPickSymbolCreatedInstrument.get() ) {
        std::cout << "IQFeedInstrumentBuild::HandleNewInstrumentRequest has wxID_OK but no instrument" << std::endl;
      }
      break;
  }

  m_pDialogPickSymbol->Destroy();
  m_pDialogPickSymbol = 0;

  return m_pDialogPickSymbolCreatedInstrument;
}

// extract this sometime because the string builder might be used elsewhere
void IQFeedInstrumentBuild::BuildInstrument( const DialogPickSymbol::DataExchange& pde, pInstrument_t& pInstrument ) {
  std::string sGenericName( pde.sIQFSymbolName );
  switch ( pde.it ) {
    case InstrumentType::Stock:
    {
      ValuesForBuildInstrument values( sGenericName, pde.sIQFeedFullName, pde.sIBSymbolName, pInstrument, 0 );
      fBuildInstrument( values );
    }
      break;
    case InstrumentType::Option:
    case InstrumentType::FuturesOption:
    {
      sGenericName = Instrument::BuildGenericOptionName( sGenericName, pde.year, pde.month + 1, pde.day, pde.os, pde.dblStrike );
      ValuesForBuildInstrument values( sGenericName, pde.sIQFeedFullName, pde.sIBSymbolName, pInstrument, pde.day );
      fBuildInstrument( values );
    }
      break;
    case InstrumentType::Future:
    {
      sGenericName = Instrument::BuildGenericFutureName( sGenericName, pde.year, pde.month + 1, pde.day );
      ValuesForBuildInstrument values( sGenericName, pde.sIQFeedFullName, pde.sIBSymbolName, pInstrument, pde.day );
      fBuildInstrument( values );
    }
      break;
  }
}

void IQFeedInstrumentBuild::HandleLookUpDescription( const std::string& sSymbol, std::string& sDescription ) {
  fLookupIQFeedDescription( sSymbol, sDescription );
}

void IQFeedInstrumentBuild::HandleComposeIQFeedFullName( DialogPickSymbol::DataExchange* pde ) {
  pde->sIQFeedFullName = "";
  pde->sIQFeedDescription = "";
  try {
    pde->sIQFeedFullName
        = ou::tf::iqfeed::BuildName(
            ou::tf::iqfeed::NameParts( pde->it, pde->sIQFSymbolName, pde->year, pde->month + 1, pde->day, pde->dblStrike, pde->os ) );
    if ( "" != pde->sIQFeedFullName ) {
      fLookupIQFeedDescription( pde->sIQFeedFullName, pde->sIQFeedDescription );
      // need instrument built at this point, as IB provides the contract # as part of the process
      // via InstrumentUpdated
      if ( "" != pde->sIQFeedDescription ) { // means we have a satisfactory iqfeed symbol
        BuildInstrument( m_de, m_pDialogPickSymbolCreatedInstrument );
      }
    }
  }
  catch ( std::runtime_error& error ) {
    std::cout << "IQFeedInstrumentBuild::HandleComposeIQFeedFullName: " << error.what() << std::endl;
  }
}

// IB has populated instrument with ContractID
void IQFeedInstrumentBuild::InstrumentUpdated( pInstrument_t pInstrument ) {
  //std::cout << "IQFeedInstrumentBuild::InstrumentUpdated ..." << std::endl;
  if ( 0 == m_pDialogPickSymbol ) {
    std::cout << "IQFeedInstrumentBuild::InstrumentUpdated warning:  no DialogPickSymbol to accept instrument" << std::endl;
  }
  else {
    if ( pInstrument.get() == m_pDialogPickSymbolCreatedInstrument.get() ) {
      // expecting contract id to already exist in instrument
      // might put a lock on instrument changes until contract comes back
      //assert( 0 != pInstrument->GetContract() );
      if ( 0 == pInstrument->GetContract() ) {
        std::cout << "IQFeedInstrumentBuild::InstrumentUpdated missing contract " << pInstrument->GetInstrumentName() << std::endl;
      }
      else {
        m_pDialogPickSymbol->UpdateContractId( pInstrument->GetContract() );
      }

    }
    else {
      std::cout << "IQFeedInstrumentBuild::InstrumentUpdated warning:  not expected instrument" << std::endl;
    }
  }
}


} // namespace tf
} // namespace ou
