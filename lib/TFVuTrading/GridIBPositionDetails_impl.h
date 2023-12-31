/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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
// Started December 30, 2015, 4:51 PM

#pragma once

#define FUSION_MAX_VECTOR_SIZE 13

#include <map>

#include <boost/fusion/container/vector/vector20.hpp>
#include <boost/fusion/include/vector20.hpp>

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/algorithm/transformation/filter.hpp>
#include <boost/fusion/include/filter.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/grid.h>  // needed for the ModelCell_macros

#include <TFVuTrading/ModelCell.h>
#include <TFVuTrading/ModelCell_ops.h>
#include <TFVuTrading/ModelCell_macros.h>

#include "GridIBPositionDetails.h"

// modelled after "PanelPortfolioPosition_impl.h", which needs to be updated for wxGrid?

namespace ou { // One Unified
namespace tf { // TradeFrame

struct GridIBPositionDetails_impl {
//public:
  GridIBPositionDetails_impl( GridIBPositionDetails& );
  virtual ~GridIBPositionDetails_impl();
//private:

// for columns: wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
#define GRID_ARRAY_PARAM_COUNT 5
#define GRID_ARRAY_COL_COUNT 12
#define GRID_ARRAY \
  (GRID_ARRAY_COL_COUNT,  \
    ( /* Col 0,            1,            2,         3,      4,             */ \
      (COL_Symbol1  , "Sym1",   wxALIGN_LEFT,   60, ModelCellString ), \
      (COL_Symbol2  , "Sym2",   wxALIGN_LEFT,  120, ModelCellString ), \
      (COL_Expiry   , "Expiry", wxALIGN_LEFT,   75, ModelCellString ), \
      (COL_Exchange , "Exchng", wxALIGN_LEFT,   70, ModelCellString ), \
      (COL_Multiple , "Mltpl",  wxALIGN_RIGHT,  50, ModelCellString ), \
      (COL_Quan     , "Quan",   wxALIGN_RIGHT,  60, ModelCellInt ), \
      (COL_Price    , "Price",  wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_Value    , "Value",  wxALIGN_RIGHT,  80, ModelCellDouble ), \
      (COL_Cost     , "Cost",   wxALIGN_RIGHT,  80, ModelCellDouble ), \
      (COL_UPNL     , "UPNL",   wxALIGN_RIGHT,  80, ModelCellDouble ), \
      (COL_RPNL     , "RPNL",   wxALIGN_RIGHT,  80, ModelCellDouble ), \
      (COL_Currency , "Crncy",  wxALIGN_LEFT,   50, ModelCellString ), \
      ) \
    ) \
  /**/

  enum {
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,GRID_EXTRACT_ENUM_LIST,0)
  };

  typedef boost::fusion::VECTOR_DEF<
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COMPOSE_MODEL_CELL,4)
  > vModelCells_t;

  class PositionDetailRow {
  public:
    PositionDetailRow( wxGrid& grid, int row ): m_grid( grid ), m_row( row ) { Init(); }
    PositionDetailRow( const PositionDetailRow& rhs ): m_grid( rhs.m_grid ), m_row( rhs.m_row ) { Init(); }
    ~PositionDetailRow( void ) {}
    void UpdateGui( void ) {
      boost::fusion::for_each( m_vModelCells, ModelCell_ops::UpdateGui( m_grid, m_row ) );
    }
    void UpdatePositionDetail( const ou::tf::ib::TWS::PositionDetail& pd ) {
      boost::fusion::at_c<COL_Symbol1>( m_vModelCells ).SetValue( pd.sSymbol );
      boost::fusion::at_c<COL_Symbol2>( m_vModelCells ).SetValue( pd.sLocalSymbol );
      boost::fusion::at_c<COL_Expiry>( m_vModelCells ).SetValue( pd.sExpiry );
      boost::fusion::at_c<COL_Exchange>( m_vModelCells ).SetValue( pd.sExchange );
      boost::fusion::at_c<COL_Multiple>( m_vModelCells ).SetValue( pd.sMultiplier );
      boost::fusion::at_c<COL_Quan>( m_vModelCells ).SetValue( pd.position );
      boost::fusion::at_c<COL_Price>( m_vModelCells ).SetValue( pd.marketPrice );
      boost::fusion::at_c<COL_Value>( m_vModelCells ).SetValue( pd.marketValue );
      boost::fusion::at_c<COL_Cost>( m_vModelCells ).SetValue( pd.averageCost );
      boost::fusion::at_c<COL_UPNL>( m_vModelCells ).SetValue( pd.unrealizedPNL );
      boost::fusion::at_c<COL_RPNL>( m_vModelCells ).SetValue( pd.realizedPNL );
      boost::fusion::at_c<COL_Currency>( m_vModelCells ).SetValue( pd.sCurrency );
      m_contract = pd.contract;
    }
  protected:
  private:
    long m_contract;
    wxGrid& m_grid;
    int m_row;
    vModelCells_t m_vModelCells;

    void Init( void ) {
      boost::fusion::fold( m_vModelCells, 0, ModelCell_ops::SetCol() );
      BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COL_ALIGNMENT,m_row)

    }
  };

  GridIBPositionDetails& m_pad; // passed in on construction

  typedef std::map<std::string,PositionDetailRow> mapPositionDetailRow_t;
  mapPositionDetailRow_t m_mapPositionDetailRow;

  void CreateControls();
  void DestroyControls();

  void UpdatePositionDetailRow( const ou::tf::ib::TWS::PositionDetail& ad );

  void OnClose( wxCloseEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //ar & boost::serialization::base_object<const TreeItemResources>(*this);
    int cnt = m_pad.GetNumberCols();
    ar & cnt;
    for ( int ix = 0; ix < cnt; ix++ ) {
      ar & m_pad.GetColSize( ix );
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //ar & boost::serialization::base_object<TreeItemResources>(*this);
    int cnt;
    ar & cnt;
    assert( cnt == m_pad.GetNumberCols() );
    int width;
    for ( int ix = 0; ix < cnt; ix++ ) {
      ar & width;
      m_pad.SetColSize( ix, width );
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

template<class Archive>
void GridIBPositionDetails::serialize(Archive & ar, const unsigned int file_version){
    ar & *m_pimpl;
}

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::GridIBPositionDetails_impl, 1)


