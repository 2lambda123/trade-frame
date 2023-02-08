/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    AppImpliedVolatility.cpp
 * Author:  raymond@burkholder.net
 * Project: ImpliedVolatility
 * Created: January 27, 2023  19:20:09
 */

// TODO: add range of options across the calendar and create volatility surface

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/sizer.h>

#include <TFTrading/Watch.h>
#include <TFTrading/BuildInstrument.h>
#include <TFTrading/ComposeInstrument.hpp>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>
#include <TFVuTrading/PanelProviderControlv2.hpp>

#include "AppImpliedVolatility.hpp"

namespace {
  static const std::string sAppName( "Implied Volatility" );
  static const std::string sVendorName( "One Unified Net Limited" );

  static const std::string sDirectory( "iv" );
  static const std::string sDbName( "ImpliedVolatility.db" );
  static const std::string sStateFileName( "ImpliedVolatility.state" );
  static const std::string sChoicesFilename( "ImpliedVolatility.cfg" );
}

// =============

IMPLEMENT_APP(AppImpliedVolatility)

bool AppImpliedVolatility::OnInit() {

  wxApp::SetVendorName( sVendorName );
  wxApp::SetAppDisplayName( sAppName );
  wxApp::SetVendorDisplayName( "(c)2023 " + sVendorName );

  wxApp::OnInit();

  if ( Load( sChoicesFilename, m_choices ) ) {
  }
  else {
    return 0;
  }

  {
    std::stringstream ss;
    auto dt = ou::TimeSource::GlobalInstance().External();
    ss
      << ou::tf::Instrument::BuildDate( dt.date() )
      << "-"
      << dt.time_of_day()
      ;
    m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
  }

  m_pFrameMain = new FrameMain( 0, wxID_ANY, sAppName );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  m_pFrameMain->SetSize( 800, 500 );
  //m_pFrameMain->SetAutoLayout( true );

  SetTopWindow( m_pFrameMain );

  wxBoxSizer* sizerFrame;
  wxBoxSizer* sizerUpper;
  wxBoxSizer* sizerLower;

  sizerFrame = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(sizerFrame);

  sizerUpper = new wxBoxSizer(wxHORIZONTAL);
  sizerUpper->SetMinSize( wxSize( 100, 50 ) );
  sizerFrame->Add( sizerUpper, 0, wxEXPAND, 2);

  m_pPanelProviderControl = new ou::tf::v2::PanelProviderControl( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
  m_pPanelProviderControl->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  m_pPanelProviderControl->Show();

  sizerUpper->Add( m_pPanelProviderControl, 0, wxALIGN_LEFT, 2);

  m_iqfeed = ou::tf::iqfeed::Provider::Factory();
  //m_iqfeed->SetThreadCount( m_choices.nThreads );

  m_pPanelProviderControl->Add(
    m_iqfeed,
    true, false, false, false,
    [](){}, // fConnecting
    [this](){ // fConnected
      ConfirmProviders();
    },
    [](){}, // fDisconnecting
    [this](){ // fDisconnected
    }
  );

  // this needs to be placed after the providers are registered
  m_pdb = std::make_unique<ou::tf::db>( sDbName ); // construct database

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
  m_pPanelLogging->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  sizerUpper->Add(m_pPanelLogging, 2, wxEXPAND, 2);

  sizerLower = new wxBoxSizer(wxVERTICAL);
  sizerFrame->Add(sizerLower, 1, wxEXPAND, 2);

  m_pWinChartView = new ou::tf::WinChartView( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
  m_pWinChartView->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

  FrameMain::vpItems_t vItems;
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

  vItems.clear();
  vItems.push_back( new mi( "Close, Done", MakeDelegate( this, &AppImpliedVolatility::HandleMenuActionCloseAndDone ) ) );
  vItems.push_back( new mi( "Save Values", MakeDelegate( this, &AppImpliedVolatility::HandleMenuActionSaveValues ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppImpliedVolatility::OnClose, this );  // start close of windows and controls

  m_pBuildInstrument = std::make_unique<ou::tf::BuildInstrument>( m_iqfeed );

  m_pFrameMain->Show( true );

  CallAfter(
    [this](){
      LoadState();
    }
  );

  return 1;
}

void AppImpliedVolatility::HandleMenuActionCloseAndDone() {
  std::cout << "Closing & Done" << std::endl;
  CallAfter(
    [this](){
      //for ( mapStrategy_t::value_type& vt: m_mapStrategy ) {
      //  vt.second->CloseAndDone();
      //}
    } );
}

void AppImpliedVolatility::HandleMenuActionSaveValues() {
  std::cout << "Saving collected values to ";
  CallAfter(
    [this](){
      m_nTSDataStreamSequence++; // sequence number on each save
      std::string sPath(
        "/app/" + sDirectory + "/" +
        m_sTSDataStreamStarted + "-" +
        boost::lexical_cast<std::string>( m_nTSDataStreamSequence ) );
      std::cout << sPath << std::endl;
      //for ( mapStrategy_t::value_type& vt: m_mapStrategy ) {
      //  vt.second->SaveWatch( sPath );
      //}
      //if ( m_pFile ) { // performed at exit to ensure no duplication in file
      //  m_pFile->Write();
      //}
      std::cout << "  ... Done " << std::endl;
    }
  );
}

void AppImpliedVolatility::ConfirmProviders() {
  if ( m_iqfeed->Connected() ) {
    ConstructUnderlying();
  }
}

void AppImpliedVolatility::ConstructUnderlying() {
  m_pComposeInstrument = std::make_shared<ou::tf::ComposeInstrument>(
    m_iqfeed,
    [this](){
      m_pComposeInstrument->Compose(
        m_choices.sSymbol,
        [this]( pInstrument_t pInstrument ){
          assert( pInstrument );
          InitializeUnderlying( pInstrument );
        }
      );
    }
  );
}

void AppImpliedVolatility::InitializeUnderlying( pInstrument_t pInstrument ) {

  const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );

  std::cout << idInstrument << std::endl;

  using pWatch_t = ou::tf::Watch::pWatch_t;
  pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );

  m_pCollector = std::make_shared<Collector>( pWatch );

}

int AppImpliedVolatility::OnExit() {
  return wxAppConsole::OnExit();
}

void AppImpliedVolatility::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppImpliedVolatility::LoadState() {
  try {
    std::cout << "Loading Config ..." << std::endl;
    std::ifstream ifs( sStateFileName );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    std::cout << "  done." << std::endl;
  }
  catch(...) {
    std::cout << "load exception" << std::endl;
  }
}

void AppImpliedVolatility::OnClose( wxCloseEvent& event ) {

  SaveState();

  if ( m_pdb ) m_pdb.reset();

  event.Skip();  // auto followed by Destroy();
}
