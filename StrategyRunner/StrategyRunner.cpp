/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// StrategyRunner.cpp : Defines the entry point for the application.
//

// 2012/03/31 The Stop-Loss-Start-Gain (SLSG) is roughly the following strategy: fix a price
// level K, if the stock price St at time t rises from < K to  K, then buy 1 share
// immediately (SG); if St drops from  K to < K, then sell immediately the share
// already hold, if any (SL); do nothing in other cases.
// -- An Infinitesimal Analysis of the Stop-Loss-Start-Gain Strategy1

#include "stdafx.h"

#include <wx/bitmap.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>

#include "StrategyRunner.h"

IMPLEMENT_APP(AppStrategyRunner)

bool AppStrategyRunner::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Strategy Simulator" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 500, 600 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 5 );

  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 1, wxEXPAND|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();

  m_pPanelOptionsParameters = new PanelOptionsParameters( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelOptionsParameters, 1, wxEXPAND, 0);
  m_pPanelOptionsParameters->Show( true );
  m_pPanelOptionsParameters->SetOnStart( MakeDelegate( this, &AppStrategyRunner::HandleBtnStart ) );
  m_pPanelOptionsParameters->SetOnStop( MakeDelegate( this, &AppStrategyRunner::HandleBtnStop ) );
  m_pPanelOptionsParameters->SetOnSave( MakeDelegate( this, &AppStrategyRunner::HandleBtnSave ) );
  m_pPanelOptionsParameters->SetOptionNearDate( boost::gregorian::date( 2012, 4, 20 ) );
  m_pPanelOptionsParameters->SetOptionFarDate( boost::gregorian::date( 2012, 6, 15 ) );

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppStrategyRunner::HandlePopulateDatabase ) );

  return 1;

}

int AppStrategyRunner::OnExit() {

//  DelinkFromPanelProviderControl();  generates stack errors

  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppStrategyRunner::HandleBtnStart( void ) {

  m_pPanelOptionsParameters->SetBtnStartEnable( false);

  std::string sDbName;

  if ( m_bExecConnected && m_bData1Connected && m_bData2Connected ) {
    if ( ou::tf::keytypes::EProviderSimulator == m_pExecutionProvider->ID() ) {
      sDbName = ":memory:";
    }
    else {
      sDbName = "StrategyTradeOptions.db";
    }

    std::stringstream ss;
    ss.str( "" );
    ss << ou::TimeSource::Instance().Internal();
    m_sTimeSamplingStarted = "/app/StrategyTradeOptions/" + ss.str();

    assert( 0 != sDbName.length() );
    m_db.Open( sDbName );

    m_pStrategyTradeOptions = new StrategyTradeOptions( m_pExecutionProvider, m_pData1Provider, m_pData2Provider );
    m_pStrategyTradeOptions->Start( 
      m_pPanelOptionsParameters->GetUnderlying(),
      m_pPanelOptionsParameters->GetOptionNearDate(), 
      m_pPanelOptionsParameters->GetOptionFarDate()
      );
    m_pPanelOptionsParameters->SetBtnStopEnable( true );
  }
  else {
    m_pPanelOptionsParameters->SetBtnStartEnable( true);
  }
}

void AppStrategyRunner::HandleBtnStop( void ) {
  m_pStrategyTradeOptions->Stop();
  m_pPanelOptionsParameters->SetBtnStartEnable( true);
}

void AppStrategyRunner::HandleBtnSave( void ) {
  if ( 0 == m_sTimeSamplingStarted.size() ) {
    std::cout << "Nothing To do." << std::endl;
  }
  else {
    m_pStrategyTradeOptions->Save( m_sTimeSamplingStarted );
  }
  
}

void AppStrategyRunner::HandlePopulateDatabase( void ) {

  ou::tf::AccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::AccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::AccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::AccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::AccountManager::pAccount_t pAccountIB
    = ou::tf::AccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountIQFeed
    = ou::tf::AccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::PortfolioManager::pPortfolio_t pPortfolio
    //= ou::tf::PortfolioManager::Instance().ConstructPortfolio( m_idPortfolio, "aoRay", "SemiAuto" );
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( "pflioOptions", "aoRay", "options" );

}

