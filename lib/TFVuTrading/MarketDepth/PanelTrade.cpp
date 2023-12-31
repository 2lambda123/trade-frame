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
 * File:    PanelTrade.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created on October 28, 2021, 15:55
 */

#include <wx/sizer.h>
//#include <wx/tooltip.h>
#include <wx/dcclient.h>

#include <TFTimeSeries/DatedDatum.h>

#include "PanelTrade.hpp"

namespace {

} // anonymous

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

namespace {
  static const unsigned int FontHeight  = 15; // pixels
  static const unsigned int RowHeight   = 18; // pixels
  static const unsigned int BorderWidth =  4; // pixels
  static const unsigned int FramedRows  = 10; // when to move into frame then recenter
}

PanelTrade::PanelTrade()
: wxWindow()
{
  Init();
};

PanelTrade::PanelTrade(
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
) {
  Init();
  Create( parent, id, pos, size, style );
}

PanelTrade::~PanelTrade() {}

void PanelTrade::Init() {

  m_bReCenter = false;

  m_nFramedRows = 0;
  m_nCenteredRows = 0;

  m_cntWinRows_Total = 0;
  m_cntWinRows_Data = 0;

  m_ixFirstPriceRow = 0; // first visible integerized price
  m_ixLastPriceRow = 0;  // last visible integerized price

  m_ixHiRecenterFrame = 0;
  m_ixLoRecenterFrame = 0;

  m_dblLastAsk = 0.0;
  m_dblLastPrice = 0.0;
  m_dblLastBid = 0.0;

  m_fClick = nullptr;

  //wxToolTip::Enable( true );
  //wxToolTip::SetDelay( 500 );
  //wxToolTip::SetAutoPop( 2000 );
  //wxToolTip::SetReshow( 1000 );

  m_timerRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &PanelTrade::HandleTimerRefresh, this, m_timerRefresh.GetId() );
  m_timerRefresh.Start( 200 ); // 5 times a second
}

bool PanelTrade::Create( /*wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style*/
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style
) {

  SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
  wxWindow::Create( parent, id, pos, size, style );

  CreateControls();
  if ( GetSizer() ) {
    GetSizer()->SetSizeHints( this );
  }

  return true;
}

void PanelTrade::CreateControls() {

  //PanelTrade* itemPanel1 = this;
  DrawWinRows();

  //Bind( wxEVT_PAINT, &PanelTrade::OnPaint, this, GetId() );

  Bind( wxEVT_SIZE, &PanelTrade::OnResize, this, GetId() );
  Bind( wxEVT_SIZING, &PanelTrade::OnResizing, this, GetId() );
  Bind( wxEVT_DESTROY, &PanelTrade::OnDestroy, this, GetId() );

}

void PanelTrade::OnPaint( wxPaintEvent& event ) {
  wxPaintDC dc( this );
}

void PanelTrade::HandleTimerRefresh( wxTimerEvent& event ) {
  if ( m_fTimer ) m_fTimer();
  //std::scoped_lock<std::mutex> lock( m_mutexTimer );
  if ( 0 < m_cntWinRows_Data ) {
    for ( int ix = m_ixFirstPriceRow; ix <= m_ixLastPriceRow; ix++ ) {
      // TODO: this requires a lookup, maybe do an iteration instead
      //   had an exception from here, segmentation fault on a WinRowElement
      //   probably stomped on from elsewhere
      m_PriceRows[ ix ].Refresh();
    }
  }
}

void PanelTrade::DrawWinRows() {

  if ( 0 != m_cntWinRows_Data ) {
    //m_pWinRow_Header.reset();
    //m_vWinRow.clear();
    //m_cntTotalWinRows = 0;
    DeleteWinRows();
  }

  wxSize sizeClient = wxWindow::GetClientSize();

  //std::scoped_lock<std::mutex> lock( m_mutexTimer );
  auto BorderWidthTimes2 = 2 * BorderWidth;
  auto Height = sizeClient.GetHeight();

  if ( Height > BorderWidthTimes2 ) {

    //assert( 0 == m_nRowCount );
    m_cntWinRows_Total = ( sizeClient.GetHeight() - 2 * BorderWidth ) / RowHeight;

    if ( 1 < m_cntWinRows_Total ) { // space enough for at least header row, and one data row

      m_cntWinRows_Data = m_cntWinRows_Total - 1; // first row is header row
      m_nFramedRows = m_cntWinRows_Data / FramedRows;
      m_nCenteredRows = ( ( m_cntWinRows_Data - m_nFramedRows ) / 2 ) - 1; // eliminates up/down jitter

      int yOffset = BorderWidth; // start offset with border

      // should this go into the vector?
      m_pWinRow_Header.reset();
      m_pWinRow_Header = WinRow::Construct( this, ou::tf::l2::rung::vElement, wxPoint( BorderWidth, yOffset ), RowHeight, true );

      yOffset += RowHeight;

      int ixWinRow = 0;
      m_vWinRow.resize( m_cntWinRows_Data );

      while ( ixWinRow < m_cntWinRows_Data ) {
        pWinRow_t pWinRow = WinRow::Construct( this, ou::tf::l2::rung::vElement, wxPoint( BorderWidth, yOffset ), RowHeight, false );
        m_vWinRow[ ixWinRow ] = pWinRow;
        yOffset += RowHeight;
        ixWinRow++;
      }

    }
    else {
      m_cntWinRows_Total = 0;
    }
  }

}

void PanelTrade::DeleteWinRows() {

  for (
    int ixWinRow = 0, ixPriceRow = m_ixFirstPriceRow;
    ixWinRow < m_cntWinRows_Data;
    ixWinRow++, ixPriceRow++
  ) {
    PriceRow& rowData( m_PriceRows[ ixPriceRow ] );
    rowData.DelRowElements();
    pWinRow_t pWinRow = std::move( m_vWinRow[ ixWinRow ] );
    //pRow->DestroyWindow();
    // delete pVRow; pWinRow auto deletes
  }
  m_cntWinRows_Total = 1;  // 0 or 1? // header row remains in tact
  m_cntWinRows_Data = 0;

  m_ixFirstPriceRow = m_ixLastPriceRow = 0;
  m_ixLoRecenterFrame = m_ixHiRecenterFrame = 0;

}

void PanelTrade::OnResize( wxSizeEvent& event ) {  // TODO: need to fix this
  CallAfter(
    [this](){
      DrawWinRows(); // probably shouldn't do this
      //ReCenterVisible( m_DataRows.Cast( m_dblLastPrice ) );
    });
  event.Skip(); // required when working with sizers
}

void PanelTrade::OnResizing( wxSizeEvent& event ) {
  //DrawWinRows();
  //m_pWinRow_Header.reset();
  //m_vWinRow.clear();
  //m_cntTotalRows = 0;
  event.Skip(); // required when working with sizers
}

void PanelTrade::OnDestroy( wxWindowDestroyEvent& event ) {

  if ( event.GetId() == GetId() ) {

    m_pWinRow_Header.reset();
    m_vWinRow.clear();

    //Unbind( wxEVT_PAINT, &PanelTrade::OnPaint, this, GetId() );

    Unbind( wxEVT_SIZE, &PanelTrade::OnResize, this, GetId() );
    Unbind( wxEVT_SIZING, &PanelTrade::OnResizing, this, GetId() );
    Unbind( wxEVT_DESTROY, &PanelTrade::OnDestroy, this, GetId() );
    Unbind( wxEVT_TIMER, &PanelTrade::HandleTimerRefresh, this, m_timerRefresh.GetId() );

    event.Skip();
  }

}

void PanelTrade::SetInterval( double interval ) {
  m_PriceRows.SetInterval( interval );
  // TODO: if called more than once, need to rebuild PriceRows with new interval
}

void PanelTrade::AppendStaticIndicator( double price, const std::string& sStatic ) {
  PriceRow& row( m_PriceRows[ price ] );
  row.AppendIndicatorStatic( sStatic );
}

void PanelTrade::UpdateDynamicIndicator( const std::string& sIndicator, double value ) {
  int ix = m_PriceRows.Cast( value );
  mapDynamicIndicator_t::iterator iter = m_mapDynamicIndicator.find( sIndicator );
  if ( m_mapDynamicIndicator.end() == iter ) {
    PriceRow& row( m_PriceRows[ ix ] );
    row.AddIndicatorDynamic( sIndicator );
    m_mapDynamicIndicator.emplace( sIndicator, ix );
  }
  else {
    if ( ix != iter->second ) {
      PriceRow& rowOld( m_PriceRows[ iter->second ] );
      rowOld.DelIndicatorDynamic( sIndicator );
      PriceRow& rowNew( m_PriceRows[ ix ] );
      rowNew.AddIndicatorDynamic( sIndicator );
      iter->second = ix;
    }
  }
}

// l1 update
void PanelTrade::OnQuote( const ou::tf::Quote& quote ) {
  // will need to use quote for tick analysis.
  // don't update the ladder, as it interferes with L2

  int ixAskPrice = m_PriceRows.Cast( quote.Ask() );
  int ixBidPrice = m_PriceRows.Cast( quote.Bid() );

  if ( 0.0 != m_dblLastAsk ) { // TODO: could keep the index instead, as it is consistent
    PriceRow& row( m_PriceRows[ m_dblLastAsk ] );
    row.SetAskVolume( false );
  }

  if ( 0.0 != m_dblLastBid ) { // TODO: could keep the index instead, as it is consistent
    PriceRow& row( m_PriceRows[ m_dblLastBid ] );
    row.SetBidVolume( false );
  }

  m_dblLastAsk = quote.Ask();
  m_dblLastBid = quote.Bid();

  if ( 0.0 != m_dblLastAsk ) {
    PriceRow& row( m_PriceRows[ m_dblLastAsk ] );
    row.SetAskVolume( true );
  }

  if ( 0.0 != m_dblLastBid ) {
    PriceRow& row( m_PriceRows[ m_dblLastBid ] );
    row.SetBidVolume( true );
  }

  int ixHiPrice = std::max( ixAskPrice, ixBidPrice );
  int ixLoPrice = std::max( ixAskPrice, ixBidPrice );
  int ixDiffPrice = ixHiPrice - ixLoPrice + 1;

  if ( m_bReCenter || ( ixDiffPrice > m_nCenteredRows ) ) {
    // maybe do this on the timer interval instead?
    int ixMidPoint = ( ixHiPrice + ixLoPrice ) / 2;
    CallAfter(
      [this, ixMidPoint](){
        ReCenterVisible( ixMidPoint );
      });
  }
  else {
    // not sure where to recenter
  }
}

// l2 update
void PanelTrade::OnQuoteAsk( double price, unsigned int volume ) {
  int ixPrice = m_PriceRows.Cast( price );
  PriceRow& row( m_PriceRows[ ixPrice ] );
  row.SetAskVolume( volume );
}

// l2 update
void PanelTrade::OnQuoteBid( double price, unsigned int volume ) {
  int ixPrice = m_PriceRows.Cast( price );
  PriceRow& row( m_PriceRows[ ixPrice ] );
  row.SetBidVolume( volume );
}

// l1 update
void PanelTrade::OnTrade( const ou::tf::Trade& trade ) {

  if ( 0.0 != m_dblLastPrice ) {
    PriceRow& row( m_PriceRows[ m_dblLastPrice ] );
    row.SetPrice( m_dblLastPrice, false ); // TODO: needs to reset back to the price, remove highlight
  }

  m_dblLastPrice = trade.Price();
  int ixPrice = m_PriceRows.Cast( m_dblLastPrice );
  PriceRow& rowData( m_PriceRows[ ixPrice ] );

  rowData.SetPrice( trade.Volume(), true ); // need to highlight the price level

  rowData.IncTicks();
  rowData.AddVolume( trade.Volume() );

  const double mid = ( m_dblLastAsk + m_dblLastBid ) / 2.0;
  if ( mid == m_dblLastPrice ) {
  }
  else {
    if ( mid < m_dblLastPrice ) {
      rowData.IncBuyCount();
      rowData.AddToBuyVolume( trade.Volume() );
    }
    else {
      rowData.IncSellCount();
      rowData.AddToSellVolume( trade.Volume() );
    }
  }

  // maybe do this on the timer interval instead?
  CallAfter(
    [this, ixPrice](){
      ReCenterVisible( ixPrice );
    });
  // TODO: add TickBuyVolume, TickSellVolume

}

void PanelTrade::ReCenterVisible( int ixPrice ) {
  // does this need a lock from background thread? - no longer, is run in foreground

  // only does something if ixPrice moves outside of window

  // TODO: check flags so changes aren't made while one thread or the other are updating
  //   maybe count how many missed opportunties presetn themeselves

  if ( 0 == ixPrice ) {
    std::cout << "PanelTrade::ReCenterVisible has 0 ixPrice" << std::endl;
  }
  else {
    if (
           m_bReCenter
      || ( ixPrice <= m_ixLoRecenterFrame )
      || ( ixPrice >= m_ixHiRecenterFrame )
      || ( m_ixLoRecenterFrame == m_ixHiRecenterFrame )
    ) {
      m_bReCenter = false;
      //std::scoped_lock<std::mutex> lock( m_mutexTimer );
      // recalibrate mappings
      if ( m_ixFirstPriceRow != m_ixLastPriceRow ) {
        // TODO: should this be performed by DeleteWinRows?
        for ( int iy = m_ixFirstPriceRow; iy <= m_ixLastPriceRow; iy++ ) {
          // remove existing string update events
          PriceRow& rowPrice( m_PriceRows[ iy ] );
          rowPrice.DelRowElements();
        }
      }
      m_ixFirstPriceRow = ixPrice - ( m_cntWinRows_Data / 2 );
      m_ixLastPriceRow = m_ixFirstPriceRow + m_cntWinRows_Data - 1;
      m_ixHiRecenterFrame = m_ixLastPriceRow - m_nFramedRows;
      m_ixLoRecenterFrame = m_ixFirstPriceRow + m_nFramedRows;
      // need to check that same numbers for each
      for (
          int ix = 0, iy = m_ixLastPriceRow;
          ix < m_cntWinRows_Data;
          ix++, iy--
        ) {
          pWinRow_t pWinRow = m_vWinRow[ ix ];
          PriceRow& rowPrice( m_PriceRows[ iy ] );
          rowPrice.SetRowElements( *pWinRow );
          rowPrice.SetPrice( m_PriceRows.Cast( iy ), false );
          fClick_t fClick( m_fClick ); // make a copy for each row
          rowPrice.Set( std::move( fClick ) );  // set each row's copy
        }
    }
  }
}

void PanelTrade::Set( fClick_t&& fClick ) {
  m_fClick = std::move( fClick );
  m_bReCenter = true;
}

void PanelTrade::SetAskQuantity( double price, int n, EColour bg ) {
  PriceRow& rowPrice( m_PriceRows[ price ] );
  rowPrice.SetAskOrderSize( n, bg );
}

void PanelTrade::SetBidQuantity( double price, int n, EColour bg ) {
  PriceRow& rowPrice( m_PriceRows[ price ] );
  rowPrice.SetBidOrderSize( n, bg );
}

void PanelTrade::UpdateProfitLoss( const int quantity, const double average ) {
  if ( 0 == quantity ) {
    m_PriceRows.ForEach(
      [](int ix, PriceRow& row){
        row.SetProfitLoss( 0.0 );
      } );
  }
  else {
    const int ixZero = m_PriceRows.Cast( average ); // find the row
    const double priceZero = m_PriceRows.Cast( ixZero ); // normalize the value

    m_PriceRows.ForEach(
      [this,quantity,priceZero](int ixRow, PriceRow& row){
        const double rowPrice = m_PriceRows.Cast( ixRow );
        const double pl = quantity * ( rowPrice - priceZero ); // multiply by multiple?
        //std::cout << "ForEach: " << ixRow << "," << rowPrice << "," << priceZero << "," << pl << "," << std::endl;
        row.SetProfitLoss( pl );
      } );
  }
}

} // market depth
} // namespace tf
} // namespace ou
