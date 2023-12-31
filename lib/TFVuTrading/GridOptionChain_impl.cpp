/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
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

#include <algorithm>
#include <functional>

#include <wx/dnd.h>
#include <wx/cursor.h>

#include "GridOptionChain_impl.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

GridOptionChain_impl::GridOptionChain_impl( GridOptionChain& details )
: wxGridTableBase()
, m_details( details ) {
}

GridOptionChain_impl::~GridOptionChain_impl() {
  m_details.SetTable( nullptr, false, wxGrid::wxGridSelectNone );
  m_vRowIX.clear();
  m_mapOptionValueRow.clear();
}

void GridOptionChain_impl::CreateControls() {

  m_details.SetDefaultColSize(50);
  m_details.SetDefaultRowSize(22);
  m_details.SetColLabelSize(22);
  m_details.SetRowLabelSize(50);

  m_details.SetTable( this, false, wxGrid::wxGridSelectCells );

  // found in ModelCell_macros.h
  #ifdef GRID_EMIT_SetColSettings
  #undef GRID_EMIT_SetColSettings
  #endif

  #define GRID_EMIT_SetColSettings( z, n, VAR ) \
    /* m_details.SetColLabelValue( VAR, _T(GRID_EXTRACT_COL_DETAILS(z, n, 1) ) ); */ \
    m_details.SetColSize( VAR++, GRID_EXTRACT_COL_DETAILS(z, n, 3) );

  int ix( 0 );
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )

  //m_details.Bind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );

  m_details.Bind( wxEVT_GRID_LABEL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );
  m_details.Bind( wxEVT_GRID_CELL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );

  m_details.Bind( wxEVT_GRID_LABEL_RIGHT_CLICK , &GridOptionChain_impl::OnGridRightClick, this );
  m_details.Bind( wxEVT_GRID_CELL_RIGHT_CLICK , &GridOptionChain_impl::OnGridRightClick, this );

  m_details.EnableDragCell( true );

  m_details.Bind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionChain_impl::OnGridCellBeginDrag, this );  // this is the event we really want

  m_details.Bind( wxEVT_MOTION, &GridOptionChain_impl::OnMouseMotion, this );  // already consumed by grid itself

  m_details.EnableEditing( false );

}

void GridOptionChain_impl::Update( double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Quote& quote ) {
  switch ( side ) {
    case ou::tf::OptionSide::Call:
      FindOptionValueRow( strike )->second.UpdateCallQuote( quote );
      break;
    case ou::tf::OptionSide::Put:
      FindOptionValueRow( strike )->second.UpdatePutQuote( quote );
      break;
    default:
      assert( false );
  }
}

void GridOptionChain_impl::Update( double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Trade& trade ) {
  switch ( side ) {
    case ou::tf::OptionSide::Call:
      FindOptionValueRow( strike )->second.UpdateCallTrade( trade );
      break;
    case ou::tf::OptionSide::Put:
      FindOptionValueRow( strike )->second.UpdatePutTrade( trade );
      break;
    default:
      assert( false );
  }
}

void GridOptionChain_impl::Update( double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Greek& greek ) {
  switch ( side ) {
    case ou::tf::OptionSide::Call:
      FindOptionValueRow( strike )->second.UpdateCallGreeks( greek );
      break;
    case ou::tf::OptionSide::Put:
      FindOptionValueRow( strike )->second.UpdatePutGreeks( greek );
      break;
    default:
      assert( false );
  }
}

void GridOptionChain_impl::Clear(  double strike ) {
  //m_mapOptionValueRow[ strike ].Init();  ?
}

void GridOptionChain_impl::Add( double strike, ou::tf::OptionSide::EOptionSide side, const std::string& sSymbol ) {

  mapOptionValueRow_iter iter = m_mapOptionValueRow.find( strike );
  if ( m_mapOptionValueRow.end() == iter ) {
    auto pair = m_mapOptionValueRow.emplace(
      mapOptionValueRow_t::value_type( strike, OptionValueRow( m_details, strike ) ) );
    assert( pair.second );
    iter = pair.first;

    struct Reindex {
      size_t ix;
      Reindex(): ix{} {}
      void operator()( OptionValueRow& row ) { row.m_nRow = ix; ix++; }
    };

    Reindex reindex;
    m_vRowIX.clear();

    for ( auto iter = m_mapOptionValueRow.rbegin(); iter != m_mapOptionValueRow.rend(); iter++ ) {
      reindex( iter->second );
      m_vRowIX.push_back( iter );
    }

    assert( m_details.InsertRows( iter->second.m_nRow ) );
  }

  switch ( side ) {
    case ou::tf::OptionSide::Call:
      iter->second.m_sCallName = sSymbol;
      break;
    case ou::tf::OptionSide::Put:
      iter->second.m_sPutName = sSymbol;
      break;
  }
}

void GridOptionChain_impl::MakeRowVisible( double strike ) {
  mapOptionValueRow_iter iter = m_mapOptionValueRow.find( strike );
  m_details.MakeCellVisible( iter->second.m_nRow, 0 );
}

GridOptionChain_impl::mapOptionValueRow_iter
GridOptionChain_impl::FindOptionValueRow( double strike ) {
  mapOptionValueRow_iter iter = m_mapOptionValueRow.find( strike );
  if ( m_mapOptionValueRow.end() == iter ) {
    assert( false );
  }
  return iter;
}

void GridOptionChain_impl::SetSelected(double strike, bool bSelected) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  wxColour colour = bSelected ? *wxWHITE : m_details.GetDefaultCellBackgroundColour();
  m_details.SetCellBackgroundColour( iter->second.m_nRow, -1, colour );
  // TODO: actually enable/disable watch?
}

void GridOptionChain_impl::Refresh() {
  m_details.ForceRefresh();
}

void GridOptionChain_impl::OnMouseMotion( wxMouseEvent& event ) {
  if ( event.Dragging() ) {

    // 2018/08/02 obsolete?  replaced by LeftClick operations?
    if ( ( 0 < m_nRow ) && ( m_nRow < m_mapOptionValueRow.size() ) ) {

      assert( m_nRow < m_vRowIX.size() );
      mapOptionValueRow_t::reverse_iterator iter = m_vRowIX[ m_nRow ];
      assert( m_mapOptionValueRow.rend() != iter );

//      std::cout << "column: " << m_nColumn << std::endl;

//      if ( ( COL_CallLast <= m_nColumn ) && ( COL_CallBid >= m_nColumn ) ) {
//        // call drag and drop
//        ou::tf::DragDropDataInstrument dndCall( iter->second.m_sCallName );
//        wxDropSource dragSource( &m_details );
//        dragSource.SetData( dndCall );
//        wxDragResult result = dragSource.DoDragDrop( true );
//      }

//      if ( ( COL_PutBid <= m_nColumn ) && ( COL_PutLast >= m_nColumn ) ) {
//        // put drag and drop
//        ou::tf::DragDropDataInstrument dndPut( iter->second.m_sPutName );
//        wxDropSource dragSource( &m_details );
//        dragSource.SetData( dndPut );
//        wxDragResult result = dragSource.DoDragDrop( true );
//      }
    }
  }
}

void GridOptionChain_impl::OnGridRightClick( wxGridEvent& event ) {
  // column header is -1, first row is 0
  //std::cout << "Right Click," << event.GetRow() << "," << event.GetCol() << std::endl;

  m_nRow = event.GetRow();
  m_nColumn = event.GetCol();

  if ( ( 0 <= m_nRow ) && ( m_nRow < m_mapOptionValueRow.size() ) ) {

    assert( m_nRow < m_vRowIX.size() );
    mapOptionValueRow_t::reverse_iterator iter = m_vRowIX[ m_nRow ];
    assert( m_mapOptionValueRow.rend() != iter );
    OptionValueRow& values( iter->second );

    switch ( m_nColumn ) {
      case COL_CallAsk: // perform buy
        std::cout << "buy 1 "
        << values.m_sCallName
        << "@" << boost::fusion::at_c<COL_CallAsk>( values.m_vModelCells ).GetText()
        << std::endl;
        if ( m_details.m_fAddToComboOrder ) {
          m_details.m_fAddToComboOrder( ou::tf::OrderSide::Buy, 1, boost::fusion::at_c<COL_CallAsk>( values.m_vModelCells ).GetValue(), values.m_sCallName );
        }
        break;
      case COL_CallBid: // perform sell
        std::cout << "sell 1 "
        << values.m_sCallName
        << "@" << boost::fusion::at_c<COL_CallBid>( values.m_vModelCells ).GetText()
        << std::endl;
        if ( m_details.m_fAddToComboOrder ) {
          m_details.m_fAddToComboOrder( ou::tf::OrderSide::Sell, 1, boost::fusion::at_c<COL_CallBid>( values.m_vModelCells ).GetValue(), values.m_sCallName );
        }
        break;
      case COL_PutBid: // perform sell
        std::cout << "sell 1 "
        << values.m_sPutName
        << "@" << boost::fusion::at_c<COL_PutBid>( values.m_vModelCells ).GetText()
        << std::endl;
        if ( m_details.m_fAddToComboOrder ) {
          m_details.m_fAddToComboOrder( ou::tf::OrderSide::Sell, 1, boost::fusion::at_c<COL_PutBid>( values.m_vModelCells ).GetValue(), values.m_sPutName );
        }
        break;
      case COL_PutAsk: // perform buy
        std::cout << "buy 1 "
        << values.m_sPutName
        << "@" << boost::fusion::at_c<COL_PutAsk>( values.m_vModelCells ).GetText()
        << std::endl;
        if ( m_details.m_fAddToComboOrder ) {
          m_details.m_fAddToComboOrder( ou::tf::OrderSide::Buy, 1, boost::fusion::at_c<COL_PutAsk>( values.m_vModelCells ).GetValue(), values.m_sPutName );
        }
        break;
    }

    // 2018/08/02 obsolete?  replaced by LeftClick operations?
//      if ( ( 0 <= m_nColumn ) && ( 5 >= m_nColumn ) ) {
//        // call drag and drop
//        ou::tf::DragDropDataInstrument dndCall( iter->second.m_sCallName );
//        wxDropSource dragSource( &m_details );
//        dragSource.SetData( dndCall );
//        wxDragResult result = dragSource.DoDragDrop( true );
//      }

//      if ( ( 7 <= m_nColumn ) && ( 12 >= m_nColumn ) ) {
//        // put drag and drop
//        ou::tf::DragDropDataInstrument dndPut( iter->second.m_sPutName );
//        wxDropSource dragSource( &m_details );
//        dragSource.SetData( dndPut );
//        wxDragResult result = dragSource.DoDragDrop( true );
//      }
  }
}

void GridOptionChain_impl::OnGridLeftClick( wxGridEvent& event ) {
  //std::cout << "Notebook Left Click: " << event.GetRow() << std::endl;
  // column header is -1, first row is 0

  m_nRow = event.GetRow();
  m_nColumn = event.GetCol();

  if ( ( 0 <= m_nRow ) && ( m_nRow < m_mapOptionValueRow.size() ) ) {

    assert( m_nRow < m_vRowIX.size() );
    mapOptionValueRow_t::reverse_iterator iterOptionValueRow = m_vRowIX[ m_nRow ];
    assert( m_mapOptionValueRow.rend() != iterOptionValueRow );

    if ( nullptr != m_details.m_fOnRowClicked ) {

      if ( ( 0 <= m_nRow ) && event.ControlDown() ) {
        ou::tf::option::Delegates call;
        call.sSymbolName = iterOptionValueRow->second.m_sCallName;
        call.fdQuote = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdateCallQuote );
        call.fdTrade = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdateCallTrade );
        call.fdGreek = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdateCallGreeks );

        ou::tf::option::Delegates put;
        put.sSymbolName = iterOptionValueRow->second.m_sPutName;
        put.fdQuote = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdatePutQuote );
        put.fdTrade = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdatePutTrade );
        put.fdGreek = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdatePutGreeks );

        iterOptionValueRow->second.m_bSelected = !iterOptionValueRow->second.m_bSelected;

        if ( m_details.m_fOnRowClicked ) {
          m_details.m_fOnRowClicked( iterOptionValueRow->first, iterOptionValueRow->second.m_bSelected, call, put );
        }
      }
    }
  }

  event.Skip( true );
}

bool GridOptionChain_impl::StartDragDrop( ou::tf::DragDropInstrument& dddi ) {

#if defined(__WXMSW__)
      wxCursor cursor( wxCURSOR_HAND );
      wxDropSource dragSource( dndCall, &m_details, cursor, cursor, cursor );
#elif defined(__WXGTK__)
      // needs icon: docs.wxwidgets.org/3.0/classwx_drop_source.html
      wxDropSource dragSource( &m_details );
#else
      assert(0);
#endif

      dragSource.SetData( dddi );
      //std::cout << "call drag start " << std::endl;
      wxDragResult result = dragSource.DoDragDrop( true );
      //std::cout << "call drag stop " << std::endl;
      switch ( result ) {
        case wxDragCopy:
        case wxDragMove:
          break;
        default:
          break;
      }
      //bSkip = false;
      return false; // bSkip
}

void GridOptionChain_impl::OnGridCellBeginDrag( wxGridEvent& event ) {
  //std::cout << "Notebook Begin Drag: " << event.GetRow() << std::endl;
  // column header is -1, first row is 0
  // use to toggle monitoring

  bool bSkip( true );

  m_nRow = event.GetRow();
  m_nColumn = event.GetCol();

  if ( ( 0 < m_nRow ) && ( m_nRow < m_mapOptionValueRow.size() ) ) {

    assert( m_nRow < m_vRowIX.size() );
    mapOptionValueRow_t::reverse_iterator iterOptionValueRow = m_vRowIX[ m_nRow ];
    assert( m_mapOptionValueRow.rend() != iterOptionValueRow );

    if ( nullptr != m_details.m_fOnOptionUnderlyingRetrieveInitiate ) {

      if ( ( 0 <= m_nColumn ) && ( 5 >= m_nColumn ) ) { // call drag and drop
        ou::tf::DragDropInstrument dndCall( [this,iterOptionValueRow]( GridOptionChain::fOnOptionUnderlyingRetrieveComplete_t&& f ){
          m_details.m_fOnOptionUnderlyingRetrieveInitiate( iterOptionValueRow->second.m_sCallName, iterOptionValueRow->first, std::move( f ) ); // iqfeed name and strike
        } );

        bSkip = StartDragDrop( dndCall );
      }

      if ( ( 7 <= m_nColumn ) && ( 12 >= m_nColumn ) ) { // put drag and drop
        ou::tf::DragDropInstrument dndPut( [this,iterOptionValueRow]( GridOptionChain::fOnOptionUnderlyingRetrieveComplete_t&& f ){
          m_details.m_fOnOptionUnderlyingRetrieveInitiate( iterOptionValueRow->second.m_sPutName, iterOptionValueRow->first, std::move( f ) ); // iqfeed name and strike
        } );

        bSkip = StartDragDrop( dndPut );
      }
    }

  }

  event.Skip( bSkip );
}

void GridOptionChain_impl::SetView( wxGrid *grid ) {
  wxGridTableBase::SetView( grid );
}

wxGrid* GridOptionChain_impl::GetView() const {
  return wxGridTableBase::GetView();
}

int GridOptionChain_impl::GetNumberRows() {
  return m_mapOptionValueRow.size();
}

int GridOptionChain_impl::GetNumberCols() {
  return GRID_ARRAY_COL_COUNT;
}

bool GridOptionChain_impl::IsEmptyCell(int row, int col ) {
  return false;
}

// https://github.com/wxWidgets/wxWidgets/blob/master/src/generic/grid.cpp
// wxGridStringTable::InsertRows
bool GridOptionChain_impl::InsertRows( size_t pos, size_t numRows ) {
  //return wxGridTableBase::InsertRows( pos, numRows ); // don't do this
    if ( GetView() ) {
      wxGridTableMessage msg(
        this,
        wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
        pos,
        numRows
      );
      GetView()->ProcessTableMessage( msg );
    }
  return true;
}

wxString GridOptionChain_impl::GetValue( int row, int col ) {

  wxString s;

  #define GRID_EMIT_SwitchGetValue( z, n, data ) \
    case GRID_EXTRACT_COL_DETAILS(z, n, 0):  \
      s = boost::fusion::at_c<GRID_EXTRACT_COL_DETAILS(z, n, 0)>( m_vRowIX[row]->second.m_vModelCells ).GetText(); \
      break;

  switch ( col ) {
    BOOST_PP_REPEAT(BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SwitchGetValue, 0 )
  }

  if ( COL_Strike == col ) {

    std::set<int> setRowsToDelete;

    // check edges of row set and disable updates
    for ( setRows_t::iterator iter = m_setRowUpdating.begin(); iter != m_setRowUpdating.end(); ++iter ) {
      if ( m_details.IsVisible( *iter, col, false ) ) {
        break;
      }
      else {
        setRowsToDelete.emplace( *iter );
      }
    }

    for ( setRows_t::reverse_iterator iter = m_setRowUpdating.rbegin(); iter != m_setRowUpdating.rend(); ++iter ) {
      if ( m_details.IsVisible( *iter, col, false ) ) {
        break;
      }
      else {
        setRowsToDelete.emplace( *iter );
      }
    }

    for ( int row: setRowsToDelete ) {
      StopStrike( row );
      m_setRowUpdating.erase( row );
    }

    // enable trade/quote/greek
    setRows_t::iterator iter = m_setRowUpdating.find( row );
    if ( m_setRowUpdating.end() == iter ) {

      auto pair = m_setRowUpdating.emplace( row );
      assert( pair.second );

      if ( m_details.m_fOptionDelegates_Attach ) {
        ou::tf::option::Delegates call;
        ou::tf::option::Delegates put;
        FillDelegates( row, call, put );
        m_details.m_fOptionDelegates_Attach( call );
        m_details.m_fOptionDelegates_Attach( put );
        std::cout
          << "start strike " << m_vRowIX[row]->first
          << "," << call.sSymbolName
          << "," << put.sSymbolName
          << std::endl;
      }
      else {
        std::cout << "start strike " << m_vRowIX[row]->first << std::endl;
      }
    }

  }

  return s;
}

void GridOptionChain_impl::FillDelegates( int row, ou::tf::option::Delegates& call, ou::tf::option::Delegates& put ) {

  OptionValueRow& ovr( m_vRowIX[row]->second );

  call.sSymbolName = ovr.m_sCallName;
  call.fdQuote = fastdelegate::MakeDelegate( &ovr, &OptionValueRow::UpdateCallQuote );
  call.fdTrade = fastdelegate::MakeDelegate( &ovr, &OptionValueRow::UpdateCallTrade );
  call.fdGreek = fastdelegate::MakeDelegate( &ovr, &OptionValueRow::UpdateCallGreeks );

  put.sSymbolName = ovr.m_sPutName;
  put.fdQuote = fastdelegate::MakeDelegate( &ovr, &OptionValueRow::UpdatePutQuote );
  put.fdTrade = fastdelegate::MakeDelegate( &ovr, &OptionValueRow::UpdatePutTrade );
  put.fdGreek = fastdelegate::MakeDelegate( &ovr, &OptionValueRow::UpdatePutGreeks );
}

void GridOptionChain_impl::StopStrike( int row ) {
  std::cout << "stop strike " << m_vRowIX[row]->first << std::endl;

  if ( m_details.m_fOptionDelegates_Detach ) { // TODO: are these called when panel grid is changed or grid destroyed?
    ou::tf::option::Delegates call;
    ou::tf::option::Delegates put;
    FillDelegates( row, call, put );
    m_details.m_fOptionDelegates_Detach( call );
    m_details.m_fOptionDelegates_Detach( put );
  }
}

void GridOptionChain_impl::SetValue(int row, int col, const wxString &value ) {
  assert( false );  // not sure if this is used
}

//void GridOptionChain_impl::SetValueAsDouble(int row, int col, double value ) {
//}

//double GridOptionChain_impl::GetValueAsDouble(int row, int col ) {
//  return 0.0;
//}

wxString GridOptionChain_impl::GetColLabelValue( int col ) {

  wxString s;

  #define GRID_EMIT_SwitchGetColLabel( z, n, data ) \
    case GRID_EXTRACT_COL_DETAILS(z, n, 0):  \
      s = wxString( GRID_EXTRACT_COL_DETAILS(z, n, 1 ) ); \
      break;

  switch ( col ) {
    BOOST_PP_REPEAT(BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SwitchGetColLabel, 0 )
  }

  return s;
}

wxGridCellAttr* GridOptionChain_impl::GetAttr (int row, int col, wxGridCellAttr::wxAttrKind kind ) {

  #define GRID_EMIT_SwitchGetColAlign( z, n, data ) \
    case GRID_EXTRACT_COL_DETAILS(z, n, 0):  \
      align = GRID_EXTRACT_COL_DETAILS(z, n, 2 ); \
      break;

  wxGridCellAttr* pAttr = new wxGridCellAttr();

  int align = wxALIGN_CENTER;
  switch ( col ) {
    BOOST_PP_REPEAT(BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SwitchGetColAlign, 0 )
  }
  pAttr->SetAlignment( align, wxALIGN_CENTER_VERTICAL );

  switch ( kind ) {
    case wxGridCellAttr::wxAttrKind::Cell:
    case wxGridCellAttr::wxAttrKind::Col:
      break;
    case wxGridCellAttr::wxAttrKind::Row:
      break;
    case wxGridCellAttr::wxAttrKind::Default:
      break;
  }

  return pAttr;

}

void GridOptionChain_impl::StopWatch() {
  std::for_each( m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(), [this](mapOptionValueRow_t::value_type& value){
    if ( value.second.m_bSelected ) {

      value.second.m_bSelected = false;

      if ( nullptr != m_details.m_fOnRowClicked ) {

        ou::tf::option::Delegates call;
        call.sSymbolName = value.second.m_sCallName;

        ou::tf::option::Delegates put;
        put.sSymbolName = value.second.m_sPutName;

        m_details.m_fOnRowClicked( value.first, value.second.m_bSelected, call, put );
      }
    }
  });
}

void GridOptionChain_impl::DestroyControls() {

  m_details.Unbind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionChain_impl::OnGridCellBeginDrag, this );

  m_details.Unbind( wxEVT_GRID_LABEL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );
  m_details.Unbind( wxEVT_GRID_CELL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );

  m_details.Unbind( wxEVT_GRID_LABEL_RIGHT_CLICK , &GridOptionChain_impl::OnGridRightClick, this );
  m_details.Unbind( wxEVT_GRID_CELL_RIGHT_CLICK , &GridOptionChain_impl::OnGridRightClick, this );

  m_details.Unbind( wxEVT_MOTION, &GridOptionChain_impl::OnMouseMotion, this );  //m_details.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );

  for ( setRows_t::iterator iter = m_setRowUpdating.begin(); iter != m_setRowUpdating.end(); ++iter ) {
    StopStrike( *iter );
  }

  m_setRowUpdating.clear();

}

} // namespace tf
} // namespace ou
