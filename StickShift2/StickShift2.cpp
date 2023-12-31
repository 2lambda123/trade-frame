/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include <functional>

// 2014/08/26  database opened on data connection, so add field to select database, switch between databases, ....

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/lexical_cast.hpp>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>

#include <TFIQFeed/BuildInstrument.h>

#include "StickShift2.h"

IMPLEMENT_APP(AppStickShift)

const std::string sFileNameMarketSymbolSubset( "../stickshift.ser" );

bool AppStickShift::OnInit() {

  //bool bExit = GetExitOnFrameDelete();
  //SetExitOnFrameDelete( true );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Manual Trading" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 5 );

  // populate variable in FrameWork01
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 0, wxEXPAND|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  m_tws->SetClientId( 1 );

  LinkToPanelProviderControl();

  m_pPanelManualOrder = new ou::tf::PanelManualOrder( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelManualOrder, 0, wxEXPAND|wxRIGHT, 5);
  m_pPanelManualOrder->Enable( false );  // portfolio isn't working properly with manual order instrument field
  m_pPanelManualOrder->Show( true );

/*
  m_pPanelOptionsParameters = new PanelOptionsParameters( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelOptionsParameters, 1, wxEXPAND, 0);
  m_pPanelOptionsParameters->Show( true );
  m_pPanelOptionsParameters->SetOnStart( MakeDelegate( this, &AppStrategyRunner::HandleBtnStart ) );
  m_pPanelOptionsParameters->SetOnStop( MakeDelegate( this, &AppStrategyRunner::HandleBtnStop ) );
  m_pPanelOptionsParameters->SetOnSave( MakeDelegate( this, &AppStrategyRunner::HandleBtnSave ) );
  m_pPanelOptionsParameters->SetOptionNearDate( boost::gregorian::date( 2012, 4, 20 ) );
  m_pPanelOptionsParameters->SetOptionFarDate( boost::gregorian::date( 2012, 6, 15 ) );
*/

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  //m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  m_db.OnRegisterTables.Add( MakeDelegate( this, &AppStickShift::HandleRegisterTables ) );
  m_db.OnRegisterRows.Add( MakeDelegate( this, &AppStickShift::HandleRegisterRows ) );
  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppStickShift::HandlePopulateDatabase ) );
  m_db.SetOnLoadDatabaseHandler( MakeDelegate( this, &AppStickShift::HandleLoadDatabase ) );

  // maybe set scenario with database and with in memory data structure
//  m_idPortfolio = boost::gregorian::to_iso_string( boost::gregorian::day_clock::local_day() ) + "StickShift";
  m_idPortfolioMaster = "master";  // keeps name constant over multiple days

  m_sDbName = "StickShift2.db";
  if ( boost::filesystem::exists( m_sDbName ) ) {
//    boost::filesystem::remove( sDbName );
  }

  m_bData1Connected = false;
  m_bExecConnected = false;
  m_bStarted = false;

  m_dblMinPL = m_dblMaxPL = 0.0;

  m_pIQFeedSymbolListOps = new ou::tf::IQFeedSymbolListOps( m_listIQFeedSymbols ); 

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "a1 New Symbol List Remote", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::ObtainNewIQFeedSymbolListRemote ) ) );
  vItems.push_back( new mi( "a2 New Symbol List Local", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::ObtainNewIQFeedSymbolListLocal ) ) );
  vItems.push_back( new mi( "a3 Load Symbol List", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::LoadIQFeedSymbolList ) ) );
  vItems.push_back( new mi( "a4 Save Symbol Subset", MakeDelegate( this, &AppStickShift::HandleMenuActionSaveSymbolSubset ) ) );
  vItems.push_back( new mi( "a5 Load Symbol Subset", MakeDelegate( this, &AppStickShift::HandleMenuActionLoadSymbolSubset ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );


  m_timerGuiRefresh.SetOwner( this );

  // need to fix this, seems to lock up program
  Bind( wxEVT_TIMER, &AppStickShift::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  m_timerGuiRefresh.Start( 250 );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppStickShift::OnClose, this );  // start close of windows and controls

  m_pPanelManualOrder->SetOnNewOrderHandler( MakeDelegate( this, &AppStickShift::HandlePanelNewOrder ) );
  m_pPanelManualOrder->SetOnSymbolTextUpdated( MakeDelegate( this, &AppStickShift::HandlePanelSymbolText ) );
  m_pPanelManualOrder->SetOnFocusPropogate( MakeDelegate( this, &AppStickShift::HandlePanelFocusPropogate ) );

  m_pFPPOE = new FrameMain( m_pFrameMain, wxID_ANY, "Portfolio Management", wxDefaultPosition, wxSize( 900, 500 ),  
    wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
    );

  m_sizerPM = new wxBoxSizer(wxVERTICAL);
  m_pFPPOE->SetSizer(m_sizerPM);

  //m_scrollPM = new wxScrolledWindow( m_pFPPOE, -1, wxDefaultPosition, wxSize(200, 400), wxVSCROLL );
  m_scrollPM = new wxScrolledWindow( m_pFPPOE, -1, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
  m_sizerPM->Add(m_scrollPM, 1, wxGROW|wxALL, 5);
  m_scrollPM->SetScrollbars(1, 1, 0, 0);

  m_sizerScrollPM = new wxBoxSizer(wxVERTICAL);
  m_scrollPM->SetSizer( m_sizerScrollPM );

  Bind( EVENT_IB_INSTRUMENT, &AppStickShift::HandleIBInstrument, this );
  
  m_pFrameMain->SetAutoLayout( true );
  m_pFrameMain->Layout();
  
  m_pFPPOE->SetAutoLayout( true );
  m_pFPPOE->Layout();
  wxPoint point = m_pFPPOE->GetPosition();
  point.x += 500;
  m_pFPPOE->SetPosition( point );
  m_pFPPOE->Show();

  
  return 1;

}

void AppStickShift::Start( void ) {
  if ( !m_bStarted ) {
    try {
      ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
      pm.OnPortfolioLoaded.Add( MakeDelegate( this, &AppStickShift::HandlePortfolioLoad ) );
      pm.OnPositionLoaded.Add( MakeDelegate( this, &AppStickShift::HandlePositionLoad ) );
      m_db.Open( m_sDbName );
      m_pFPPOE->Update();
      //m_pFPPOE->Refresh();
      //m_pFPPOE->SetAutoLayout( true );
      m_pFPPOE->Layout();  
      m_bStarted = true;
    }
    catch (...) {
      std::cout << "problems with AppStickShift::Start" << std::endl;
    }
  }
}

void AppStickShift::HandleMenuActionSaveSymbolSubset( void ) {

  m_vExchanges.clear();
  m_vExchanges.insert( "NYSE" );
  //m_vExchanges.push_back( "NYSE_AMEX" );
  m_vExchanges.insert( "NYSE,NYSE_ARCA" );
  m_vExchanges.insert( "NASDAQ,NGSM" );
  m_vExchanges.insert( "NASDAQ,NGM" );
  m_vExchanges.insert( "OPRA" );
  m_vExchanges.insert( "TSE" );
  //m_vExchanges.push_back( "NASDAQ,NMS" );
  //m_vExchanges.push_back( "NASDAQ,SMCAP" );
  //m_vExchanges.push_back( "NASDAQ,OTCBB" );
  //m_vExchanges.push_back( "NASDAQ,OTC" );
  //m_vExchanges.insert( "CANADIAN,TSE" );  // don't do yet, simplifies contract creation for IB

  m_vClassifiers.clear();
  m_vClassifiers.insert( ou::tf::IQFeedSymbolListOps::classifier_t::Equity );
  m_vClassifiers.insert( ou::tf::IQFeedSymbolListOps::classifier_t::IEOption );

  std::cout << "Subsetting symbols ... " << std::endl;
  ou::tf::iqfeed::InMemoryMktSymbolList listIQFeedSymbols;
  ou::tf::IQFeedSymbolListOps::SelectSymbols selection( m_vClassifiers, listIQFeedSymbols );
  m_listIQFeedSymbols.SelectSymbolsByExchange( m_vExchanges.begin(), m_vExchanges.end(), selection );
  std::cout << "  " << listIQFeedSymbols.Size() << " symbols in subset." << std::endl;

  //std::string sFileName( sFileNameMarketSymbolSubset );
  std::cout << "Saving subset to " << sFileNameMarketSymbolSubset << " ..." << std::endl;
//  listIQFeedSymbols.HandleParsedStructure( m_listIQFeedSymbols.GetTrd( m_sNameUnderlying ) );
//  m_listIQFeedSymbols.SelectOptionsByUnderlying( m_sNameOptionUnderlying, listIQFeedSymbols );
  listIQFeedSymbols.SaveToFile( sFileNameMarketSymbolSubset );  // __.ser
  std::cout << " ... done." << std::endl;

  // next step will be to add in the options for the underlyings selected.
}

void AppStickShift::HandleMenuActionLoadSymbolSubset( void ) {
  //std::string sFileName( sFileNameMarketSymbolSubset );
  std::cout << "Loading From " << sFileNameMarketSymbolSubset << " ..." << std::endl;
  m_listIQFeedSymbols.LoadFromFile( sFileNameMarketSymbolSubset );  // __.ser
  std::cout << "  " << m_listIQFeedSymbols.Size() << " symbols loaded." << std::endl;
}

void AppStickShift::HandlePortfolioLoad( pPortfolio_t& pPortfolio ) {
  namespace ph = std::placeholders;
  assert( 0 );  // 20171001 don't think this will work properly without the events handled properly
  //   ie, need to fix the assignment to m_fConstructPosition
  //ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  m_pLastPPP = new ou::tf::PanelPortfolioPosition( m_scrollPM );
  m_sizerScrollPM->Add( m_pLastPPP, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 0);
  //pPPP->SetPortfolio( pm.GetPortfolio( idPortfolio ) );
  m_pLastPPP->SetPortfolio( pPortfolio );
  //m_pLastPPP->SetNameLookup( MakeDelegate( this, &AppStickShift::LookupDescription ) );
  //m_pLastPPP->SetConstructPosition( MakeDelegate( this, &AppStickShift::ConstructEquityPosition0 ) ); // 20171001 need this to be called
  //m_pLastPPP->m_fConstructPosition = std::bind( &AppStickShift::ConstructEquityPosition1, this, ph::_1 ); // 20171001 probably won't work as ...0 wasn't called
  //m_pLastPPP->SetConstructPortfolio( MakeDelegate( this, &AppStickShift::HandleConstructPortfolio ) );
  m_pLastPPP->m_fConstructPortfolio = std::bind( &AppStickShift::HandleConstructPortfolio, this, ph::_1, ph::_2, ph::_3 );
  m_mapPortfolios.insert( mapPortfolios_t::value_type( pPortfolio->Id(), structPortfolio( m_pLastPPP ) ) );
}

void AppStickShift::HandlePositionLoad( pPosition_t& pPosition ) {
  m_pLastPPP->AddPosition( pPosition );
}

void AppStickShift::HandleGuiRefresh( wxTimerEvent& event ) {
  for ( mapPortfolios_t::iterator iter = m_mapPortfolios.begin(); m_mapPortfolios.end() != iter; ++iter ) {
    iter->second.pPPP->UpdateGui();
  }
}

void AppStickShift::LookupDescription( const std::string& sSymbolName, std::string& sDescription ) {
  sDescription = "";
  try {
    const ou::tf::iqfeed::InMemoryMktSymbolList::trd_t& trd( m_listIQFeedSymbols.GetTrd( sSymbolName ) );
    sDescription = trd.sDescription;
  }
  catch ( std::runtime_error& e ) {
  }
}

void AppStickShift::ConstructEquityPosition0( const std::string& sName, pPortfolio_t pPortfolio, fAddPosition_t function ) {

  m_EquityPositionCallbackInfo.pPortfolio = pPortfolio;
  m_EquityPositionCallbackInfo.function = function;
  
  // test symbol:  QGCZ15P1200
  // test symbol:  GLD1531X120

  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() );
  ou::tf::Instrument::pInstrument_t pInstrument;  //empty instrument
  
  // 20151025
  // can pull equity, future, option out of iqfeed marketsymbols file
  // construct basic instrument based upon iqfeed info 
  // then redo the following to obtain the contract info from ib and add to the instrument
  
  // 20151026 
  // maybe get rid of the whole underlying instrument requirement in an option symbol (similar to fact that futures option doesn't have underlying tie in )
  // requires a bunch of rewrite, but probably worth it

  // sName is going to be an IQFeed name from the MarketSymbols file, so needs to be in main as well as alternatesymbolsnames list
  if ( im.Exists( sName, pInstrument ) ) {  // the call will supply instrument if it exists
    ConstructEquityPosition1( pInstrument );
  }
  else {
    // this is going to have to be changed to reflect various symbols types recovered from the IQF Market Symbols file
    // which might be simplified if IB already has the code for interpreting a pInstrument_t

    bool bConstructed( false );
    try {
      const ou::tf::iqfeed::InMemoryMktSymbolList::trd_t& trd( m_listIQFeedSymbols.GetTrd( sName ) );
      
      switch ( trd.sc ) {
        case ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier::Equity:
        case ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier::Future: 
        case ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier::FOption:
          pInstrument = ou::tf::iqfeed::BuildInstrument( sName, trd );  // need to check that correct name is supplied
          break;
        case ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier::IEOption: 
          {
          ou::tf::Instrument::pInstrument_t pInstrumentUnderlying;
          if ( im.Exists( trd.sUnderlying, pInstrumentUnderlying ) ) {
            }
          else {
            const ou::tf::iqfeed::InMemoryMktSymbolList::trd_t& trdUnderlying( m_listIQFeedSymbols.GetTrd( trd.sUnderlying ) );
            switch (trdUnderlying.sc ) {
              case ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier::Equity:
                pInstrumentUnderlying = ou::tf::iqfeed::BuildInstrument( sName, trdUnderlying );  // build the underlying in preparation for the option   // need to check that correct name is supplied
                im.Register( pInstrumentUnderlying );
                break;
              default:
                throw std::runtime_error( "ConstructEquityPosition0: no applicable instrument type for underlying" );
              }
            }
          pInstrument = ou::tf::iqfeed::BuildInstrument( sName, trd  );  // build an option   // need to check that correct name is supplied
          }
          break;
        default:
          throw std::runtime_error( "ConstructEquityPosition0: no applicable instrument type" );
        }
      //im.Register( pInstrument ); // register once info returned from IB instead
      bConstructed = true;
    }
    catch ( std::runtime_error& e ) {
      std::cout << "Couldn't find symbol: " + sName << std::endl;
    }
    if ( bConstructed ) {
      std::cout << "Requesting IB: " << sName << std::endl;
      // rewrite:  TWS should have mechanism to convert instrument to request
      //ou::tf::IBTWS::Contract contract;
      //contract.currency = "USD";
      //contract.exchange = "SMART";
      //contract.secType = "STK";
      //contract.symbol = sName;
      m_tws->RequestContractDetails( 
        sName, pInstrument,  // need to check that correct name is supplied
        //contract, 
        // HandleIBContractDetails will submit an event which ends up at ConstructEquityPosition1
        MakeDelegate( this, &AppStickShift::HandleIBContractDetails ), 
        MakeDelegate( this, &AppStickShift::HandleIBContractDetailsDone ) 
        );
    }
  }
}

void AppStickShift::HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument ) {
  QueueEvent( new EventIBInstrument( EVENT_IB_INSTRUMENT, -1, pInstrument ) );
  // this is in another thread, and should be handled in a thread safe manner.
}

void AppStickShift::HandleIBContractDetailsDone( void ) {
}

void AppStickShift::HandleIBInstrument( EventIBInstrument& event ) {
  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() );
  im.Register( event.GetInstrument() ); // by stint of being here, should be new, and therefore registerable (not true, as IB info might be added to iqfeed info)
  ConstructEquityPosition1( event.GetInstrument() );
}

// 20151025 problem with portfolio is 0 in m_EquityPositionCallbackInfo
void AppStickShift::ConstructEquityPosition1( pInstrument_t& pInstrument ) {
  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance().Instance() );
  try {
    pPosition_t pPosition( pm.ConstructPosition( 
      m_EquityPositionCallbackInfo.pPortfolio->GetRow().idPortfolio,
      pInstrument->GetInstrumentName(),
      "",
      this->m_pExecutionProvider->GetName(),
      this->m_pData1Provider->GetName(),
      //"ib01", "iq01", 
      this->m_pExecutionProvider, this->m_pData1Provider,
      pInstrument ) 
      );
    if ( 0 != m_EquityPositionCallbackInfo.function ) {
      m_EquityPositionCallbackInfo.function( pPosition );
    }
  }
  catch ( std::runtime_error& e ) {
    std::cout << "Position Construction Error:  " << e.what() << std::endl;
  }
}

void AppStickShift::HandleConstructPortfolio( ou::tf::PanelPortfolioPosition& ppp,const std::string& sPortfolioId, const std::string& sDescription ) {
  // check if portfolio exists
  if ( ou::tf::PortfolioManager::Instance().PortfolioExists( sPortfolioId ) ) {
    std::cout << "PortfolioId " << sPortfolioId << " already exists." << std::endl;
  }
  else {
    ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
      sPortfolioId, "aoRay", ppp.GetPortfolio()->Id(),ou::tf::Portfolio::Standard, ppp.GetPortfolio()->GetRow().sCurrency, sDescription );
  }
}

void AppStickShift::HandleRegisterTables(  ou::db::Session& session ) {
}

void AppStickShift::HandleRegisterRows(  ou::db::Session& session ) {
}

void AppStickShift::HandlePopulateDatabase( void ) {

  ou::tf::AccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::AccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::AccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::AccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::AccountManager::pAccount_t pAccountIB
    = ou::tf::AccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountIQFeed
    = ou::tf::AccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountSimulator
    = ou::tf::AccountManager::Instance().ConstructAccount( "sim01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderSimulator, "Sim", "acctid", "login", "password" );

  std::string sNull;

  m_pPortfolioMaster
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
      m_idPortfolioMaster, "aoRay", sNull, ou::tf::Portfolio::Master, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "StickShift" );

  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    ou::tf::Currency::Name[ ou::tf::Currency::USD ], "aoRay", m_idPortfolioMaster, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Currency Monitor" );
//  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::CAD ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::CAD ], "Currency Monitor" );
//  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::EUR ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::EUR ], "Currency Monitor" );
//  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::AUD ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::AUD ], "Currency Monitor" );
//  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::GBP ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::GBP ], "Currency Monitor" );
    
}

void AppStickShift::HandleLoadDatabase( void ) {
    ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
    pm.LoadActivePortfolios();
}

void AppStickShift::HandlePanelNewOrder( const ou::tf::PanelManualOrder::Order_t& order ) {
  try {
    ou::tf::InstrumentManager& mgr( ou::tf::InstrumentManager::Instance() );
    //pInstrument_t pInstrument = m_vManualOrders[ m_curDialogManualOrder ].pInstrument;
    pInstrument_t pInstrument = m_IBInstrumentInfo.pInstrument;
    if ( !mgr.Exists( pInstrument ) ) {
      mgr.Register( pInstrument );
    }
//    if ( 0 == m_pPosition.get() ) {
//      m_pPosition = ou::tf::PortfolioManager::Instance().ConstructPosition( 
//        m_idPortfolioMaster, pInstrument->GetInstrumentName(), "manual", "ib01", "ib01", m_pExecutionProvider, m_pData1Provider, pInstrument );
//    }
    ou::tf::OrderManager& om( ou::tf::OrderManager::Instance() );
    ou::tf::OrderManager::pOrder_t pOrder;
    switch ( order.eOrderType ) {
    case ou::tf::OrderType::Market: 
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity );
//      m_pPosition->PlaceOrder( ou::tf::OrderType::Market, order.eOrderSide, order.nQuantity );
      break;
    case ou::tf::OrderType::Limit:
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1 );
//      m_pPosition->PlaceOrder( ou::tf::OrderType::Limit, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      break;
    case ou::tf::OrderType::Stop:
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1 );
//      m_pPosition->PlaceOrder( ou::tf::OrderType::Stop, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      break;
    }
    //ou::tf::OrderManager::pOrder_t pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1, order.dblPrice2 );
    //om.PlaceOrder( m_tws.get(), pOrder );
  }
  catch (...) {
    int i = 1;
  }
}

void AppStickShift::HandlePanelSymbolText( const std::string& sName ) {
  // need to fix to handle equity, option, future, etc.  merge with code from above so common code usage
  // 2014/09/30 maybe need to disable this panel, as the order doesn't land in an appropriate portfolio or position.
  ou::tf::IBTWS::Contract contract;
  contract.currency = "USD";
  contract.exchange = "SMART";
  contract.secType = "STK";
  contract.symbol = sName;
  // IB responds only when symbol is found, bad symbols will not illicit a response
  m_pPanelManualOrder->SetInstrumentDescription( "" );
  m_tws->RequestContractDetails( 
    contract, 
    MakeDelegate( this, &AppStickShift::HandleIBContractDetails ), MakeDelegate( this, &AppStickShift::HandleIBContractDetailsDone ) );
}

void AppStickShift::HandlePanelFocusPropogate( unsigned int ix ) {
}

void AppStickShift::OnClose( wxCloseEvent& event ) {
//  pm.OnPortfolioLoaded.Remove( MakeDelegate( this, &AppStickShift::HandlePortfolioLoad ) );
//  pm.OnPositionLoaded.Remove( MakeDelegate( this, &AppStickShift::HandlePositionLoaded ) );
  m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

int AppStickShift::OnExit() {
    
  // called after destroying all application windows

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  pm.OnPortfolioLoaded.Remove( MakeDelegate( this, &AppStickShift::HandlePortfolioLoad ) );

//  DelinkFromPanelProviderControl();  generates stack errors
  m_timerGuiRefresh.Stop();
  if ( m_db.IsOpen() ) m_db.Close();

//  delete m_pCPPOE;
//  m_pCPPOE = 0;

  return wxApp::OnExit();
}

void AppStickShift::OnData1Connected( int ) {
  m_bData1Connected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
    Start();
  }
}

void AppStickShift::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
    Start();
  }
}

void AppStickShift::OnData1Disconnected( int ) {
  m_bData1Connected = false;
}

void AppStickShift::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}
