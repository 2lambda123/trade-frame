/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    AppAutoTrade.cpp
 * Author:  raymond@burkholder.net
 * Project: AutoTrade
 * Created: February 14, 2022 10:06
 */

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/sizer.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Position.h>
#include <TFTrading/Portfolio.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>

#include "Config.h"
#include "Strategy.h"
#include "AppAutoTrade.h"

namespace {
  static const std::string sAppName( "Auto Trade Example" );
  static const std::string sConfigFilename( "AutoTrade.cfg" );
  static const std::string sStateFileName( "AutoTrade.state" );
  static const std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
}

IMPLEMENT_APP(AppAutoTrade)

bool AppAutoTrade::OnInit() {

  wxApp::SetAppDisplayName( sAppName );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c)2022 One Unified Net Limited" );

  wxApp::OnInit();

  config::Options options;

  if ( Load( sConfigFilename, options ) ) {
    m_sSymbol = options.sSymbol;
  }
  else {
    return 0;
  }

  {
    std::stringstream ss;
    auto dt = ou::TimeSource::Instance().External();
    ss
      << ou::tf::Instrument::BuildDate( dt.date() )
      << " "
      << dt.time_of_day()
      ;
    m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
  }

  m_pFrameMain = new FrameMain( 0, wxID_ANY, sAppName );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

    wxBoxSizer* sizerFrame;
    wxBoxSizer* sizerLeft;
    wxBoxSizer* sizerRight;

  // Sizer for FrameMain
    sizerFrame = new wxBoxSizer(wxHORIZONTAL);
    m_pFrameMain->SetSizer(sizerFrame);

  // Sizer for Controls, Logging
    sizerLeft = new wxBoxSizer(wxVERTICAL);
    sizerFrame->Add(sizerLeft, 0, wxGROW, 2);

    // m_pPanelProviderControl
    m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_pPanelProviderControl->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    sizerLeft->Add(m_pPanelProviderControl, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    // m_pPanelLogging
    m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_pPanelLogging->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    sizerLeft->Add(m_pPanelLogging, 1, wxGROW, 2);

    sizerRight = new wxBoxSizer(wxHORIZONTAL);
    sizerFrame->Add(sizerRight, 1, wxGROW, 2);

    // m_pPanelChart
    m_pWinChartView = new ou::tf::WinChartView( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_pWinChartView->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    sizerRight->Add(m_pWinChartView, 1, wxGROW, 2);

  LinkToPanelProviderControl();

  std::cout << "symbol: " << m_sSymbol << std::endl;

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  ou::tf::Instrument::pInstrument_t pInstrument;
  pInstrument = std::make_shared<ou::tf::Instrument>( options.sSymbol ); // simple for an iqfeed watch
  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, options.sSymbol );
  pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, this->m_pData1Provider ); // will need to be iqfeed provider, check?
  pPosition_t pPosition = std::make_shared<ou::tf::Position>( pWatch, m_pExecutionProvider );

  m_pWinChartView->SetChartDataView( &m_ChartDataView );
  m_pStrategy = std::make_unique<Strategy>( pPosition, m_ChartDataView, options );

  m_pFrameMain->SetAutoLayout( true );
  m_pFrameMain->Layout();
  m_pFrameMain->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppAutoTrade::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects
  //vItems.push_back( new mi( "c1 Start Watch", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionStartWatch ) ) );
  //vItems.push_back( new mi( "c2 Stop Watch", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionStopWatch ) ) );
  //vItems.push_back( new mi( "d1 Start Chart", MakeDelegate( this, &AppRdafL1::HandleMenuActionStartChart ) ) );
  //vItems.push_back( new mi( "d2 Stop Chart", MakeDelegate( this, &AppRdafL1::HandleMenuActionStopChart ) ) );
  vItems.push_back( new mi( "e1 Save Values", MakeDelegate( this, &AppAutoTrade::HandleMenuActionSaveValues ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  if ( !boost::filesystem::exists( sTimeZoneSpec ) ) {
    std::cout << "Required file does not exist:  " << sTimeZoneSpec << std::endl;
  }

  CallAfter(
    [this](){
      LoadState();
    }
  );

  return 1;
}

void AppAutoTrade::HandleMenuActionSaveValues( void ) {
  std::cout << "Saving collected values ... " << std::endl;
  CallAfter(
    [this](){
      //m_pInteractiveChart->SaveWatch( "/app/AutoTrade/" + m_sTSDataStreamStarted );
      std::cout << "  ... Done " << std::endl;
    }
  );
}

int AppAutoTrade::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors

  return wxAppConsole::OnExit();
}

void AppAutoTrade::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose

  //m_pWinChartView->SetChartDataView( nullptr, false );
  //delete m_pChartData;
  //m_pChartData = nullptr;

  //m_pFrameControls->Close();

  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  SaveState();
  event.Skip();  // auto followed by Destroy();
}

void AppAutoTrade::OnData1Connected( int ) {
  m_bData1Connected = true;
//  AutoStartCollection();
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }

  // ** Note:  turn on iqfeed only, symbols not set for IB yet
  // TODO: need to fix this with start / stop

}

void AppAutoTrade::OnData2Connected( int ) {
  m_bData2Connected = true;
//  AutoStartCollection();
  if ( m_bData2Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppAutoTrade::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppAutoTrade::OnData1Disconnected( int ) {
  m_bData1Connected = false;
}

void AppAutoTrade::OnData2Disconnected( int ) {
  m_bData2Connected = false;
}

void AppAutoTrade::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}

void AppAutoTrade::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppAutoTrade::LoadState() {
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