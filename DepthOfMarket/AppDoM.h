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

#pragma once

/*
 * File:    AppDoM.h
 * Author:  raymond@burkholder.net
 * Project: App Depth of Market
 * Created: October 12, 2021, 23:04
 */

#include <atomic>
#include <memory>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/app.h>

#include <TFTrading/Order.h>
#include <TFTrading/Watch.h>
#include <TFTrading/Position.h>
#include <TFTrading/DBWrapper.h>
#include <TFTrading/Execution.h>
#include <TFTrading/Portfolio.h>

#include <TFIndicators/TSEMA.h>
#include <TFIndicators/Pivots.h>
#include <TFIndicators/TSSWStochastic.h>

#include <TFIQFeed/HistoryRequest.h>

#include <TFIQFeed/Level2/Symbols.hpp>
#include <TFIQFeed/Level2/FeatureSet.hpp>

#include <TFBitsNPieces/FrameWork01.h>

#include <TFVuTrading/PanelLogging.h>

#include <TFVuTrading/MarketDepth/PriceLevelOrder.hpp>
#include <TFVuTrading/MarketDepth/ExecutionControl.hpp>

#include "Config.h"
#include "PanelStatistics.hpp"

class FrameMain;

namespace ou {
namespace tf {
  class FrameControls;
  class BuildInstrument;
namespace l2 {
  class PanelTrade;
  class PanelSideBySide;
  class PanelLevelIIButtons;
} // namespace l2
} // namespace tf
} // namespace ou

class AppDoM:
  public wxApp,
  public ou::tf::FrameWork01<AppDoM>
{
  friend ou::tf::FrameWork01<AppDoM>;
  friend class boost::serialization::access;
public:
protected:
private:

  using pOrder_t = ou::tf::Order::pOrder_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  std::string m_sTSDataStreamStarted;

  config::Options m_config;

  std::unique_ptr<ou::tf::db> m_pdb;

  FrameMain* m_pFrameMain;
  ou::tf::FrameControls* m_pFrameControls;

  ou::tf::PanelLogging* m_pPanelLogging;
  ou::tf::l2::PanelTrade* m_pPanelTrade;
  ou::tf::l2::PanelLevelIIButtons* m_pPanelLevelIIButtons;
  ou::tf::l2::PanelSideBySide* m_pPanelSideBySide;

  std::shared_ptr<ou::tf::l2::ExecutionControl> m_pExecutionControl;

  int m_cntLoops;
  PanelStatistics::values_t m_valuesStatistics;
  PanelStatistics* m_pPanelStatistics;

  ou::tf::iqfeed::HistoryRequest::pHistoryRequest_t m_pHistoryRequest;

  std::unique_ptr<ou::tf::BuildInstrument> m_pBuildInstrument;

  bool m_bRecordDepth;
  ou::tf::DepthsByOrder m_depths_byorder; // time series for persistence

  std::atomic_uint32_t m_nMarketOrdersAsk;
  std::atomic_uint32_t m_nMarketOrdersBid;

  bool m_bTriggerFeatureSetDump;

  ou::tf::iqfeed::l2::OrderBased m_OrderBased; // direct access
  ou::tf::iqfeed::l2::FeatureSet m_FeatureSet;
  std::unique_ptr<ou::tf::iqfeed::l2::Symbols> m_pDispatch;

  pWatch_t m_pWatch;
  pPosition_t m_pPosition;
  pPortfolio_t m_pPortfolio;

  ou::tf::Bars m_barsHistory;
  ou::tf::PivotSet m_setPivots;

  struct Stochastic {

    using pTSSWStochastic_t = std::unique_ptr<ou::tf::TSSWStochastic>;
    using fStochastic_t = std::function<void(ptime,double,double,double)>;
    // datetime, percentage, price min, price max

    pTSSWStochastic_t m_pIndicatorStochastic;

    Stochastic(
      ou::tf::Quotes& quotes, int nPeriods, time_duration td
    , fStochastic_t&& fStochastic
    ) {
      m_pIndicatorStochastic = std::make_unique<ou::tf::TSSWStochastic>(
        quotes, nPeriods, td, std::move( fStochastic )
      );
    }

    Stochastic( const Stochastic& ) = delete;
    Stochastic( Stochastic&& rhs ) = delete;

    ~Stochastic() {
      m_pIndicatorStochastic.reset();
    }

  };

  using pStochastic_t = std::unique_ptr<Stochastic>;
  using vStochastic_t = std::vector<pStochastic_t>;
  vStochastic_t m_vStochastic;

  struct MA {

    std::string sName;
    ou::tf::hf::TSEMA<ou::tf::Quote> m_ema;

    MA( ou::tf::Quotes& quotes, size_t nPeriods, time_duration tdPeriod, const std::string& sName_ )
    : sName( sName_ ), m_ema( quotes, nPeriods, tdPeriod ) {}

    MA( MA&& rhs )
    : m_ema( std::move( rhs.m_ema ) ), sName( std::move( rhs.sName ) )
    {}

    double Latest() const { return m_ema.GetEMA(); }
  };

  using vMA_t = std::vector<MA>;
  vMA_t m_vMA;

  double m_dblLastAsk;
  double m_dblLastBid;

  using mapOrders_t = std::map<double,ou::tf::l2::PriceLevelOrder>;
  // note only one side can have orders at any moment in time
  mapOrders_t m_mapAskOrders;
  mapOrders_t m_mapBidOrders;

  void EmitMarketMakerMaps();

  virtual bool OnInit();
  void OnClose( wxCloseEvent& event );
  virtual int OnExit();

  void OnData1Connected( int );
  void OnData1Disconnecting( int );
  void OnData1Disconnected( int );

  void OnExecConnected( int );
  void OnExecDisconnected( int );

  void BuildPosition();
  void InitializePosition( pInstrument_t );

  void StartDepthByMM();
  void StartDepthByOrder();
  void StartDepthByOrderWithFVS();

  void OnFundamentals( const ou::tf::Watch::Fundamentals& );
  void OnQuote( const ou::tf::Quote& );
  void OnTrade( const ou::tf::Trade& );

  void LoadDailyHistory();

  void MenuItem_RecordWatch_Start();
  void MenuItem_RecordWatch_Status();
  void MenuItem_RecordWatch_Stop();
  void MenuItem_RecordWatch_Save();

  void MenuItem_FeatureSet_Dump();

  void HandleArmedFlag( bool );

  void SaveState();
  void LoadState();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & *m_pFrameMain;
    ar & *m_pFrameControls;
}

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & *m_pFrameMain;
    if ( 2 <= version ) {
      ar & *m_pFrameControls;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(AppDoM, 2)

DECLARE_APP(AppDoM)
