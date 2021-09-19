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

#pragma once

#include <set>
#include <map>
#include <string>
#include <functional>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/treebase.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/PortfolioManager.h>

#include <TFIndicators/Pivots.h>

#include <TFIQFeed/IQFeedProvider.h>
#include <TFInteractiveBrokers/IBTWS.h>
//#include <TFSimulation/SimulationProvider.h>

#include "Underlying.h"
#include "ManageStrategy.h"
#include "AcquireFundamentals.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
class OptionChainQuery;
} // namespace iqfeed
namespace option {
class Engine;
} // namespace option
} // namespace tf
} // namespace ou

class wxMenu;
class HistoryRequest;


class MasterPortfolio {
  friend class boost::serialization::access;
public:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using pPortfolio_t =  ou::tf::PortfolioManager::pPortfolio_t;
  using pPosition_t = ou::tf::PortfolioManager::pPosition_t;
  using idPortfolio_t = ou::tf::PortfolioManager::idPortfolio_t;

  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  using trd_t = ou::tf::iqfeed::MarketSymbol::TableRowDef;
  using fGetTableRowDef_t = std::function<const trd_t&(const std::string& sIQFeedSymbolName)>;
  using fOptionDefinition_t = ManageStrategy::fOptionDefinition_t;
  using fGatherOptionDefinitions_t = ManageStrategy::fGatherOptionDefinitions_t;
  using fConstructPositionUnderlying_t = ManageStrategy::fConstructPosition_t;

  using fChartRoot_t = std::function<wxTreeItemId(const std::string&,pChartDataView_t)>;
  using fChartAdd_t = std::function<wxTreeItemId(wxTreeItemId,const std::string&,pChartDataView_t,wxMenu*)>;
  using fChartDel_t = std::function<void(wxTreeItemId)>;

  MasterPortfolio(
    pPortfolio_t pMasterPortfolio,
    pProvider_t pExec, pProvider_t pData1, pProvider_t pData2,
    fGatherOptionDefinitions_t &&,
    fGetTableRowDef_t &&,
    fChartRoot_t&&, fChartAdd_t&&, fChartDel_t&&
    );
  ~MasterPortfolio();

  void Add( pPortfolio_t ); // from database load
  void Add( pPosition_t );  // from database load

  void Load( ptime dtLatestEod );

  double UpdateChart();

  void ClosePositions( void );
  void SaveSeries( const std::string& sPath );

  void Test( void );

  void TakeProfits();
  void CloseExpiryItm( boost::gregorian::date );
  void CloseFarItm();
  void CloseForProfits();
  void CloseItmLeg();
  void AddCombo( bool bForced );
  void EmitInfo();

protected:
private:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using pOrder_t = ou::tf::Order::pOrder_t;

  using pProviderIBTWS_t = ou::tf::IBTWS::pProvider_t;
  using pProviderIQFeed_t = ou::tf::iqfeed::IQFeedProvider::pProvider_t;
  //using pProviderSim_t =  ou::tf::SimulationProvider::pProvider_t;

  using pInstrument_t =ou::tf::IBTWS::pInstrument_t;

  std::string m_sTSDataStreamStarted;

  bool m_bStarted;

  ptime m_dtLatestEod;

  ou::tf::DatedDatum::volume_t m_nSharesTrading;

  std::thread m_worker;

  pProvider_t m_pExec;
  pProvider_t m_pData1;
  pProvider_t m_pData2;

  pProviderIBTWS_t m_pIB;
  pProviderIQFeed_t m_pIQ;
  //pProviderSim_t n_pSim;

  pPortfolio_t m_pMasterPortfolio;

  ou::tf::LiborFromIQFeed m_libor;
  ou::tf::FedRateFromIQFeed m_fedrate;
  std::unique_ptr<ou::tf::option::Engine> m_pOptionEngine;

  pChartDataView_t m_pChartDataView;

  ou::ChartEntryIndicator m_cePLCurrent;
  ou::ChartEntryIndicator m_cePLUnRealized;
  ou::ChartEntryIndicator m_cePLRealized;
  ou::ChartEntryIndicator m_ceCommissionPaid;

  wxTreeItemId m_idTreeRoot;
  wxTreeItemId m_idTreeUnderlying;
  wxTreeItemId m_idTreeStrategies;
  //wxTreeItemId m_idTreeOptions;

  using pManageStrategy_t = std::shared_ptr<ManageStrategy>;

  struct Strategy {
    wxTreeItemId idTreeItem;
    pManageStrategy_t pManageStrategy;
    ou::tf::Price::price_t priceOpen;
    double dblBestProbability;
    bool bChartActivated;
    Strategy( pManageStrategy_t pManageStrategy_ )
    : pManageStrategy( std::move( pManageStrategy_ ) ),
      bChartActivated( false ),
      priceOpen {}, dblBestProbability {}
    {}
    Strategy()
    :
      bChartActivated( false ),
      priceOpen {}, dblBestProbability {}
    {}
    void Set( pManageStrategy_t&& pManageStrategy_ ) { pManageStrategy = std::move( pManageStrategy_ ); }
  };

  struct Statistics {
    ou::tf::PivotSet setPivots;
  };

  using pUnderlying_t = std::unique_ptr<Underlying>;
  using pStrategy_t = std::unique_ptr<Strategy>;
  using mapStrategy_t = std::map<idPortfolio_t,pStrategy_t>;
  using iterStrategy_t = mapStrategy_t::iterator;

  struct UnderlyingWithStrategies {
    wxTreeItemId idTreeItem;
    pUnderlying_t pUnderlying;
    Statistics statistics;
    pStrategy_t pStrategyInWaiting;
    mapStrategy_t mapStrategyActive;
    mapStrategy_t mapStrategyClosed;
    ou::tf::Bars m_barsHistory;

    UnderlyingWithStrategies( pUnderlying_t pUnderlying_ )
    : pUnderlying( std::move( pUnderlying_ ) ) {}
    //UnderlyingWithStrategies( const Statistics&& statistics_ )
    //: statistics( std::move( statistics_ ) ) {}
    void ClosePositions() {
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        pStrategy->pManageStrategy->ClosePositions();
      }
    }
    void SaveSeries( const std::string& sPrefix ) {
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        pStrategy->pManageStrategy->SaveSeries( sPrefix );
      }
    }
    double EmitInfo() {
      double sum {};
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        sum += pStrategy->pManageStrategy->EmitInfo();
      }
      return sum;
    }
    void CloseForProfits() {
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        pStrategy->pManageStrategy->CloseForProfits();
      }
    }
    void TakeProfits() {
      for ( mapStrategy_t::value_type& vt: mapStrategyActive ) {
        pStrategy_t& pStrategy( vt.second );
        pStrategy->pManageStrategy->TakeProfits();
      }
    }
  };

  using mapUnderlyingWithStrategies_t = std::map<std::string /* underlying */, UnderlyingWithStrategies>;
  using iterUnderlyingWithStrategies_t = mapUnderlyingWithStrategies_t::iterator;
  mapUnderlyingWithStrategies_t m_mapUnderlyingWithStrategies;

  // cache of portfolios and positions for use when building strategy instances
  using mapPosition_t = std::map<std::string,pPosition_t>;
  using mapPosition_iter = mapPosition_t::iterator;
  using mapPortfolio_t = std::map<idPortfolio_t,pPortfolio_t>;
  using mapPortfolio_iter = mapPortfolio_t::iterator;

  struct StrategyCache {
    // stuff during database load goes here temporarily
    bool m_bAccessed;
    pPortfolio_t m_pPortfolio;  // portfolio for the strategy
    mapPortfolio_t m_mapPortfolio; // sub-portfolios (option combos) -> recursive lookup [or could be a set or vector?]
    mapPosition_t m_mapPosition; // positions associated with portfolio
    StrategyCache( pPortfolio_t pPortfolio )
    : m_bAccessed( false ),
      m_pPortfolio( pPortfolio )
    {}
    StrategyCache( const StrategyCache&& rhs )
    : m_bAccessed( rhs.m_bAccessed ),
      m_pPortfolio( std::move( rhs.m_pPortfolio ) ),
      m_mapPosition( std::move( rhs.m_mapPosition ) )
    {}
  };

  using mapStrategyCache_t = std::map<ou::tf::Portfolio::idPortfolio_t,StrategyCache>;
  using mapStrategyCache_iter = mapStrategyCache_t::iterator;
  mapStrategyCache_t m_mapStrategyCache;

  using fInstrument_t = std::function<void(pInstrument_t)>;
  using fInstrumentDone_t = std::function<void(const std::string&)>;

  class Details { // redo with enable_shared_from_this ?
  public:

    using pAcquireFundamentals_t = AcquireFundamentals::pAcquireFundamentals_t;
    using fBuildInstrument_t = std::function<void(const std::string&, Details&, fInstrument_t&&, fInstrumentDone_t&& )>;

    Details( fBuildInstrument_t&& fBuildInstrument )
    : m_fBuildInstrument( std::move( fBuildInstrument ) )
    {
      assert( nullptr != m_fBuildInstrument );
    }

    void Add( const std::string&& sSymbol ) {
      std::lock_guard<std::mutex> lock( m_mutexDetails );
      //std::cout << "Details::Add " << sSymbol << "," << m_setSymbol.size() << std::endl;
      setSymbol_t::const_iterator iter = m_setSymbolWO.find( sSymbol );
      if ( m_setSymbolWO.end() == iter ) {
        m_setSymbolWO.emplace( sSymbol );
        m_setSymbol.emplace( std::move( sSymbol ) );
      }
      else {
        std::cout << "Details::Add " << sSymbol << " exists in m_setSymbolWO" << std::endl;
      }
      Update();
    }

    void Add( const std::string& sSymbol, pAcquireFundamentals_t& pAcquireFundamentals ) {
      //std::lock_guard<std::mutex> lock( m_mutexDetails );
      //assert( !m_mutexDetails.try_lock() ); // lock needs to be already in place
      m_mapAcquisition.emplace( std::make_pair( sSymbol, pAcquireFundamentals ) );
    }

    void Clear() {
      std::lock_guard<std::mutex> lock( m_mutexDetails );
      assert( 0 == m_mapAcquisition.size() );
      m_setSymbol.clear();
      //m_mapDetail.clear();
    }

    // TOOD: add a done item to move to next underlying

  private:

    using setSymbol_t = std::set<std::string>;
    using mapAcquisition_t = std::map<std::string,pAcquireFundamentals_t>;

    setSymbol_t m_setSymbolWO;
    setSymbol_t m_setSymbol;
    mapAcquisition_t m_mapAcquisition;

    fBuildInstrument_t m_fBuildInstrument;

    std::mutex m_mutexDetails;

    void Update() { // prerequisite: lock is set, can this be asserted?
      //assert( !m_mutexDetails.try_lock() ); // lock needs to be already in place
      if ( 5 > m_mapAcquisition.size() ) {
        if ( 0 != m_setSymbol.size() ) {
          setSymbol_t::iterator iter = m_setSymbol.begin();
          const std::string sSymbol = std::move( *iter );
          m_setSymbol.erase( iter );
          //std::cout << "Details::Update::BuildInstrument " << sSymbol << std::endl;
          m_fBuildInstrument(
            sSymbol,
            *this,
            [sSymbol](pInstrument_t pInstrument){
              //std::cout << "Details::Update instrument " << sSymbol << std::endl;
              //std::lock_guard<std::mutex> lock( m_mutexDetails );
              //m_mapDetail.emplace(
              //  std::make_pair( sSymbol, pInstrument )
              //);
            },
            [this](const std::string& sSymbol){
              //std::cout << "Details::Update done " << sSymbol << std::endl;
              std::lock_guard<std::mutex> lock( m_mutexDetails );
              mapAcquisition_t::const_iterator iter = m_mapAcquisition.find( sSymbol );
              if ( m_mapAcquisition.end() != iter ) {
                m_mapAcquisition.erase( iter );
              }
              Update();
            });
        }
      }
    }

  };

  void BuildInstrument( const std::string& sIQFeedSymbol, Details&, fInstrument_t&&, fInstrumentDone_t&& );

  std::unique_ptr<Details> m_pDetailsUnderlying;
  std::unique_ptr<Details> m_pDetailsOptions;

  // ---

  void ProcessOptionChainElement( pInstrument_t );
  // ---

  //using mapVolatility_t = std::multimap<double, std::string>; // string is name of instrument
  //mapVolatility_t m_mapVolatility;

  fGatherOptionDefinitions_t m_fOptionNamesByUnderlying;
  fGetTableRowDef_t m_fGetTableRowDef;
  fChartRoot_t m_fChartRoot;
  fChartAdd_t m_fChartAdd;
  fChartDel_t m_fChartDel;

  // used by Load
  using setSymbols_t = std::set<std::string>;
  setSymbols_t m_setSymbols;
  setSymbols_t::const_iterator m_iterSymbols;

  using mapSpecs_t = std::map<std::string,ManageStrategy::Specs>;
  static const mapSpecs_t m_mapSpecs;

  std::unique_ptr<ou::tf::iqfeed::OptionChainQuery> m_pOptionChainQuery; // need to disconnect
  std::unique_ptr<HistoryRequest> m_pHistoryRequest;  // TODO: need to disconnect

  void ProcessSymbolList();
  void AddUnderlying( pWatch_t );

  using fConstructedWatch_t  = std::function<void(pWatch_t)>;
  pManageStrategy_t ConstructStrategy( const std::string& sUnderlying, pPortfolio_t pPortfolioUnderlying );
  void StartStrategies( const std::string& sUnderlying, UnderlyingWithStrategies& );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

BOOST_CLASS_VERSION(MasterPortfolio, 1)