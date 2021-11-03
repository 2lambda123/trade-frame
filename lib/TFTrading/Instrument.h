/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

// should a provider be included?  No, because this allows the same instrument
//  to be used to supply details to any provider.

// 20151227 removing references underlying in option instruments, option instruments should be able to stand by themselves
//   can use the iqfeed market symbols list to find cross references between underlying and list of options

#pragma once

#include <map>
#include <memory>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian_calendar.hpp>

#include <OUCommon/Delegate.h>

#include <OUSQL/Functions.h>

#include "TradingEnumerations.h"
#include "KeyTypes.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class Instrument {
public:

  using eidProvider_t = keytypes::eidProvider_t;  // from ProviderInterfaceBase in ProviderInterface.h
  using idExchange_t = keytypes::idExchange_t;
  using idInstrument_t = keytypes::idInstrument_t;
  using idInstrument_cref = const idInstrument_t&;
  using pInstrument_t = std::shared_ptr<Instrument>;
  using pInstrument_cref = const pInstrument_t&;

  using mapExchangeRule_t = std::map<std::string,int>; // pair<exchange,ruleid>

  static std::string BuildDate( boost::gregorian::date );
  static std::string BuildDate( uint16_t year, uint16_t month, uint16_t day );
  static std::string BuildGenericOptionName( const std::string& sBaseName, uint16_t year, uint16_t month, uint16_t day, OptionSide::enumOptionSide side, double strike );
  static std::string BuildGenericOptionName( const std::string& sBaseName, boost::gregorian::date, OptionSide::enumOptionSide side, double strike );
  static std::string BuildGenericFutureName( const std::string& sBaseName, uint16_t year, uint16_t month, uint16_t day );
  static std::string BuildGenericFutureName( const std::string& sBaseName, boost::gregorian::date );

  // To consider: use boost::gregorian::date or use a date_t(year,month,day) structure?
  //   then conversion in and out of boost::gregorian::date is not required

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "instrumentid", idInstrument );
      ou::db::Field( a, "type", eType );
      ou::db::Field( a, "description", sDescription );
      ou::db::Field( a, "exchangeid", idExchange );
//      ou::db::Field( a, "underlyingid", idUnderlying );
      ou::db::Field( a, "currency", eCurrency );
      ou::db::Field( a, "countercurrency", eCounterCurrency );
      ou::db::Field( a, "optionside", eOptionSide );
      ou::db::Field( a, "year", nYear );
      ou::db::Field( a, "month", nMonth );
      ou::db::Field( a, "day", nDay );
      ou::db::Field( a, "strike", dblStrike );
      ou::db::Field( a, "ibcontract", nIBContract );
      ou::db::Field( a, "multiplier", nMultiplier );
      ou::db::Field( a, "mintick", dblMinTick );
      ou::db::Field( a, "sigdigits", nSignificantDigits );
      ou::db::Field( a, "exchange_rules", sExchangeRules ); // comma separated names
    }

    idInstrument_t idInstrument; // main name
    InstrumentType::enumInstrumentType eType;
    std::string sDescription;
    idExchange_t idExchange;
//    idInstrument_t idUnderlying;  // used only for when loading from db and need to compare assigned underlying
    Currency::enumCurrency eCurrency;  // base currency - http://en.wikipedia.org/wiki/Currency_pair
    Currency::enumCurrency eCounterCurrency; // quote/counter currency -  - depicts how many units of the counter currency are needed to buy one unit of the base currency
    OptionSide::enumOptionSide eOptionSide;
    boost::uint16_t nYear; // future, option
    boost::uint16_t nMonth; // future, option
    boost::uint16_t nDay; // future, option
    double dblStrike;
    boost::int32_t nIBContract; // used with CIBTWS
    boost::uint32_t nMultiplier;  // number of units per contract: stk 1x, option 100x
    double dblMinTick;
    boost::uint8_t nSignificantDigits;
    std::string sExchangeRules;  // from ib, one or more exchange:ruleid

    TableRowDef( void ) // default constructor
      : eType( InstrumentType::Unknown ), eCurrency( Currency::USD ), eCounterCurrency( Currency::USD ),
      eOptionSide( OptionSide::Unknown ), nYear( 0 ), nMonth( 0 ), nDay( 0 ), dblStrike( 0.0 ),
      nIBContract( 0 ), nMultiplier( 1 ), dblMinTick( 0.01 ), nSignificantDigits( 2 ) {};
    TableRowDef( // strictly for obtaining fundamentals
      idInstrument_t idInstrument_)
      : idInstrument( idInstrument_ ), eType( InstrumentType::enumInstrumentType::Unknown ), idExchange( "" ),
      eCurrency( Currency::USD ), eCounterCurrency( Currency::USD ),
      eOptionSide( OptionSide::Unknown ), nYear( 0 ), nMonth( 0 ), nDay( 0 ), dblStrike( 0.0 ),
      nIBContract( 0 ), nMultiplier( 1 ), dblMinTick( 0.01 ), nSignificantDigits( 2 ) {
        //assert( eType < InstrumentType::_Count );
        //assert( eType > InstrumentType::Unknown );
        assert( 0 < idInstrument.size() );
    };
    TableRowDef( // equity / generic creation
      idInstrument_t idInstrument_, InstrumentType::enumInstrumentType eType_, idExchange_t idExchange_ )
      : idInstrument( idInstrument_ ), eType( eType_ ), idExchange( idExchange_ ),
      eCurrency( Currency::USD ), eCounterCurrency( Currency::USD ),
      eOptionSide( OptionSide::Unknown ), nYear( 0 ), nMonth( 0 ), nDay( 0 ), dblStrike( 0.0 ),
      nIBContract( 0 ), nMultiplier( 1 ), dblMinTick( 0.01 ), nSignificantDigits( 2 ) {
        assert( eType < InstrumentType::_Count );
        assert( eType > InstrumentType::Unknown );
        assert( 0 < idInstrument.size() );
    };
    TableRowDef( // future
      idInstrument_t idInstrument_, InstrumentType::enumInstrumentType eType_, idExchange_t idExchange_,
      boost::uint16_t nYear_, boost::uint16_t nMonth_, boost::uint16_t nDay_ = 0 )
      : idInstrument( idInstrument_ ), eType( eType_ ), idExchange( idExchange_ ),
      eCurrency( Currency::USD ), eCounterCurrency( Currency::USD ),
      eOptionSide( OptionSide::Unknown ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_ ), dblStrike( 0.0 ),
      nIBContract( 0 ), nMultiplier( 1 ), dblMinTick( 0.01 ), nSignificantDigits( 2 ) {
        assert( eType == InstrumentType::Future  );
        assert( 0 < idInstrument.size() );
    };
    TableRowDef( // option/futuresoption with yymm [TODO with day available now, remove this?]
      idInstrument_t idInstrument_, InstrumentType::enumInstrumentType eType_, idExchange_t idExchange_,
//      idInstrument_t idUnderlying_,
      boost::uint16_t nYear_, boost::uint16_t nMonth_,
      OptionSide::enumOptionSide eOptionSide_, double dblStrike_  )
      : idInstrument( idInstrument_ ), eType( eType_ ), idExchange( idExchange_ ),
//      idUnderlying( idUnderlying_ ),
      eCurrency( Currency::USD ), eCounterCurrency( Currency::USD ),
      eOptionSide( eOptionSide_ ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( 0 ), dblStrike( dblStrike_ ),
      nIBContract( 0 ), nMultiplier( 100 ), dblMinTick( 0.01 ), nSignificantDigits( 2 ) {
        assert( ( OptionSide::Call == eOptionSide_ ) || ( OptionSide::Put == eOptionSide_ ) );
        assert( ( eType_ == InstrumentType::Option )
             || ( eType_ == InstrumentType::FuturesOption ) );
        assert( 0 < idInstrument.size() );
//        assert( 0 < idUnderlying.size() );
    };
    TableRowDef( // option/futuresoption with yymmdd
      idInstrument_t idInstrument_, InstrumentType::enumInstrumentType eType_, idExchange_t idExchange_,
//      idInstrument_t idUnderlying_,
      boost::uint16_t nYear_, boost::uint16_t nMonth_, boost::uint16_t nDay_,
      OptionSide::enumOptionSide eOptionSide_, double dblStrike_  )
      : idInstrument( idInstrument_ ), eType( eType_ ), idExchange( idExchange_ ),
//      idUnderlying( idUnderlying_ ),
      eCurrency( Currency::USD ), eCounterCurrency( Currency::USD ),
      eOptionSide( eOptionSide_ ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_ ), dblStrike( dblStrike_ ),
      nIBContract( 0 ), nMultiplier( 100 ), dblMinTick( 0.01 ), nSignificantDigits( 2 ) {
        assert( ( OptionSide::Call == eOptionSide_ ) || ( OptionSide::Put == eOptionSide_ ) );
        assert( ( eType_ == InstrumentType::Option )
             || ( eType_ == InstrumentType::FuturesOption ) );
        assert( 0 < idInstrument.size() );
//        assert( 0 < idUnderlying.size() );
    };
    TableRowDef( // currency
      const idInstrument_t& idInstrument_, const idInstrument_t& idCounterInstrument_,
      InstrumentType::enumInstrumentType eType_, idExchange_t idExchange_,
      Currency::enumCurrency eCurrency_, Currency::enumCurrency eCounterCurrency_ )
      : idInstrument( idInstrument_ ), eType( eType_ ), idExchange( idExchange_ ),
//        idUnderlying( idCounterInstrument_ ),
	eCurrency( eCurrency_ ), eCounterCurrency( eCounterCurrency_ ),
        eOptionSide( OptionSide::Unknown ), nYear( 0 ), nMonth( 0 ), nDay( 0 ), dblStrike( 0.0 ),
        nIBContract( 0 ), nMultiplier( 1 ), dblMinTick( 0.00005 ), nSignificantDigits( 5 ) {
          assert( eType_ == InstrumentType::Currency );
          assert( 0 < idInstrument.size() );
//          assert( 0 < idUnderlying.size() );
    };
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "instrumentid" );
      ou::db::Constraint( a, "exchangeid", tablenames::sExchange, "exchangeid" );
//      ou::db::Constraint( a, "underlyingid", tablenames::sInstrument, "instrumentid" );  // what happens with empty string?
    }
  };
  Instrument( const TableRowDef& row );  // regular instruments
//  Instrument( const TableRowDef& row, pInstrument_t& pUnderlying ); // options, futuresoptions
  Instrument( idInstrument_cref idInstrument ); // just enough to obtain more info via fundamentals
  Instrument( // equity / generic creation
    idInstrument_cref idInstrument, InstrumentType::enumInstrumentType type,
    const idExchange_t& sExchangeName
     );
  Instrument(   // future
    idInstrument_cref idInstrument, InstrumentType::enumInstrumentType type,
    const idExchange_t& sExchangeName,
    boost::uint16_t year, boost::uint16_t month, boost::uint16_t day = 0 );
  Instrument(   // option with yymm  -- like what is done on the future, merge yymm and yymmdd together
    idInstrument_cref sInstrumentName, InstrumentType::enumInstrumentType type,
    const idExchange_t& sExchangeName,
    boost::uint16_t year, boost::uint16_t month,
//    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side,
    double strike );
  Instrument(   // option with yymmdd
    idInstrument_cref sInstrumentName, InstrumentType::enumInstrumentType type,
    const idExchange_t& sExchangeName,
    boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
//    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side,
    double strike );
  Instrument(  // currency
    const idInstrument_t& idInstrument,
//    const idInstrument_t& idCounterInstrument,
    InstrumentType::enumInstrumentType eType, const idExchange_t& idExchange,
    Currency::enumCurrency base, Currency::enumCurrency counter );

  virtual ~Instrument();

  idInstrument_cref GetInstrumentName( void ) const { return m_row.idInstrument; };
//  idInstrument_cref GetUnderlyingName( void );

  idInstrument_cref GetInstrumentName( eidProvider_t id ) const;
//  idInstrument_cref GetUnderlyingName( eidProvider_t id );

  void SetAlternateName( eidProvider_t, idInstrument_cref );

  struct AlternateNameChangeInfo_t {
    eidProvider_t id;
    const std::string& s1;
    const std::string& s2;
    AlternateNameChangeInfo_t( eidProvider_t id_, const std::string& s1_, const std::string& s2_ ):
      id( id_ ), s1( s1_ ), s2( s2_ ) {};
  };
  ou::Delegate<const AlternateNameChangeInfo_t&> OnAlternateNameAdded;  // idProvider, key, alt
  ou::Delegate<const AlternateNameChangeInfo_t&> OnAlternateNameChanged;  // idProvider, old, new
  template<typename F> void ScanAlternateNames( F f ) {
    for ( mapAlternateNames_t::const_iterator iter = m_mapAlternateNames.begin(); m_mapAlternateNames.end() != iter; ++iter ) {
      f( iter->first, iter->second, m_row.idInstrument );
    }
  };

  InstrumentType::enumInstrumentType GetInstrumentType( void ) const { return m_row.eType; };
  bool IsCurrency( void )      const { return ( InstrumentType::Currency == m_row.eType ); };
  bool IsStock( void )         const { return ( InstrumentType::Stock == m_row.eType ); };
  bool IsOption( void )        const { return ( InstrumentType::Option == m_row.eType ); };
  bool IsFuture( void )        const { return ( InstrumentType::Future == m_row.eType ); };
  bool IsFuturesOption( void ) const { return ( InstrumentType::FuturesOption == m_row.eType ); };

  void SetExchangeName( const std::string& sExchangeName ) { m_row.idExchange = sExchangeName; }
  const std::string& GetExchangeName( void ) const { return m_row.idExchange; };

  void SetCurrency( Currency::enumCurrency eCurrency ) { m_row.eCurrency = eCurrency; };
  const char *GetCurrencyName( void ) const { return Currency::Name[ m_row.eCurrency ]; };

  double GetStrike( void ) const { return m_row.dblStrike; };
  boost::uint16_t GetExpiryYear( void ) const { return m_row.nYear; };
  boost::uint16_t GetExpiryMonth( void ) const { return m_row.nMonth; };
  boost::uint16_t GetExpiryDay( void ) const { return m_row.nDay; };
  boost::gregorian::date GetExpiry( void ) const { return boost::gregorian::date( m_row.nYear, m_row.nMonth, m_row.nDay ); };
  std::string GetExpiryAsIsoString( void ) const { return boost::gregorian::to_iso_string( GetExpiry() ); };
  boost::posix_time::ptime GetExpiryUtc( void ) const;

  OptionSide::enumOptionSide GetOptionSide( void ) const { return m_row.eOptionSide; };

  // these may not be needed anymore
  void SetCommonCalcExpiry( boost::gregorian::date date ) { m_dateCommonCalc = date; };  // kludge for options with actual expiry on Friday, but dated Saturday
  boost::gregorian::date GetCommonCalcExpiry( void ) const { return m_dateCommonCalc; };
  std::string GetCommonCalcExpiryAsIsoString( void ) const { return boost::gregorian::to_iso_string( m_dateCommonCalc ); }

  void SetContract( boost::int32_t id ) { m_row.nIBContract = id; };  // for Interactive Brokers contract identification
  boost::int32_t GetContract( void ) const { return m_row.nIBContract; };

  void SetMultiplier( boost::uint32_t nMultiplier ) { m_row.nMultiplier = nMultiplier; };
  boost::uint32_t GetMultiplier( void ) const { return m_row.nMultiplier; };

  void SetMinTick( double dblMinTick ) { m_row.dblMinTick = dblMinTick; };
  double GetMinTick( void ) const { return m_row.dblMinTick; };
  double NormalizeOrderPrice( double price ) const;
  static double NormalizeOrderPrice( double price, double interval );

  void SetSignificantDigits( boost::uint8_t nSignificantDigits ) { m_row.nSignificantDigits = nSignificantDigits; };
  boost::uint8_t GetSignificantDigits( void ) const { return m_row.nSignificantDigits; };

  typedef boost::posix_time::time_period dtrMarketOpenClose_t;
  void SetTimeLiquid( const boost::posix_time::ptime& dtOpen, const boost::posix_time::ptime& dtClose ) { m_dtrTimeLiquid = dtrMarketOpenClose_t( dtOpen, dtClose ); };
  const dtrMarketOpenClose_t& GetTimeLiquid( void ) const { return m_dtrTimeLiquid; };

  void SetTimeTrading( const boost::posix_time::ptime& dtOpen, const boost::posix_time::ptime& dtClose ) { m_dtrTimeTrading = dtrMarketOpenClose_t( dtOpen, dtClose ); };
  const dtrMarketOpenClose_t& GetTimeTrading( void ) const { return m_dtrTimeTrading; };

  void SetExchangeRules( const std::string& sExchangeRules ) { m_row.sExchangeRules = sExchangeRules; }
  int GetExchangeRule();

  bool operator==( const Instrument& rhs ) const;

  const TableRowDef& GetRow( void ) const { return m_row; };

protected:
private:

  TableRowDef m_row;

  using mapAlternateNames_t = std::map<eidProvider_t, idInstrument_t>;
  mapAlternateNames_t m_mapAlternateNames;

  boost::gregorian::date m_dateCommonCalc;

  mapExchangeRule_t m_mapExchangeRule;

  dtrMarketOpenClose_t m_dtrTimeLiquid;
  dtrMarketOpenClose_t m_dtrTimeTrading;

  Instrument( const Instrument& );  // copy ctor
  Instrument& operator=( const Instrument& ); // assignement

};

} // namespace tf
} // namespace ou
