/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:    Strangle.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created on May 25, 2019, 10:56 PM
 */

#include <array>

#include "LegDef.h"
#include "Strangle.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

namespace {

  static const size_t nLegs( 2 );
  static const boost::gregorian::days nDaysToExpiry( 1 );

  using LegDef = ou::tf::option::LegDef;
  using rLegDef_t = std::array<LegDef,nLegs>;

  static const rLegDef_t m_rLegDefLong = {
    LegDef( LegDef::EOrderSide::Buy, 1, LegDef::EOptionSide::Call, 0.10 ), // upper
    LegDef( LegDef::EOrderSide::Buy, 1, LegDef::EOptionSide::Put,  0.10 )  // lower
  };
  static const rLegDef_t m_rLegDefShort = {
    LegDef( LegDef::EOrderSide::Sell, 1, LegDef::EOptionSide::Call, 0.10 ), // upper
    LegDef( LegDef::EOrderSide::Sell, 1, LegDef::EOptionSide::Put,  0.10 )  // lower
  };

} // namespace anon

Strangle::Strangle()
: Combo()
{
}

Strangle::Strangle( const Strangle& rhs )
: Combo( rhs )
{}

Strangle::Strangle( const Strangle&& rhs )
: Combo( std::move( rhs ) )
{}

Strangle::~Strangle() {
}

void Strangle::Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) {
  Combo::Tick( bInTrend, dblPriceUnderlying, dt ); // first or last in sequence?
  CheckStop( dblPriceUnderlying );
//  if ( m_vLeg.empty() ) {
//    ChooseStrikes( dblPriceUnderlying );
//  }
}

// TODO: need to fix this if other legs present.  Need to limit to the active legs.
//   maybe vector of inactive legs
// NOTE: if volatility drops, then losses occur on premium
void Strangle::PlaceOrder( ou::tf::OrderSide::enumOrderSide side ) {
  switch ( m_state ) {
    case State::Positions: // doesn't confirm both put/call are available
    case State::Watching:
      for ( Leg& leg: m_vLeg ) {
        leg.PlaceOrder( side, 1 );
      }
      m_state = State::Executing;
      break;
  }
}

// TODO: functional, three/four sections:
//   choose option type, and strike price
//   check if different from existing trackers
//   construct options, and place into tracker
//   retrieve options after validation, and reset validator

// TODO: should be able to construct so leg1 + leg2 credit > 1.00

/* static */ void Strangle::ChooseLegs( // throw Chain exceptions
    double slope,
    const mapChains_t& chains,
    boost::gregorian::date date, double price,
    fLegSelected_t&& fLegSelected
)
{

  citerChain_t citerChain =
    Combo::SelectChain( chains, date, nDaysToExpiry );

  const ou::tf::option::Chain& chain( citerChain->second );

  double strikeOtmCall {};
  double strikeOtmPut {};

  strikeOtmCall = chain.Call_Otm( price );
  assert( 0.0 <= ( strikeOtmCall - price ) );
  if ( ( 0.20 * 0.50 ) > ( strikeOtmCall - price ) ) { // within edge of range
    strikeOtmCall = chain.Call_Otm( strikeOtmCall ); // choose a further out strike
  }
  strikeOtmPut = chain.Put_Otm( price );
  assert( 0.0 <= ( price - strikeOtmPut ) );
  if ( ( 0.20 * 0.50 ) > ( price - strikeOtmPut ) ) { // within edge of range
    strikeOtmPut = chain.Put_Otm( strikeOtmPut ); // choose a further out strike
  }
  assert( strikeOtmCall > strikeOtmPut );
  const double dblStrikeDelta = strikeOtmCall - strikeOtmPut;
  if ( m_dblMaxStrangleDelta > dblStrikeDelta ) {
    const double dblExclusionRange = 0.5 * ( ( 1.0 - 0.20 ) * dblStrikeDelta );  // enter in middle 20% only
    if (
      ( price < ( strikeOtmCall - dblExclusionRange ) ) &&
      ( price > ( strikeOtmPut  + dblExclusionRange ) )
    ) {
    }
    else{
      std::stringstream ss;
      ss << "Strangle::ChooseStrikes " << price << "," << dblExclusionRange << "," << strikeOtmCall << "," << strikeOtmPut;
      throw exception_strike_range_exceeded( ss.str().c_str() );
    }
  }

  if ( 0.0 <= slope ) {
    fLegSelected( m_rLegDefLong[0].dblSpread, strikeOtmCall, citerChain->first, chain.GetIQFeedNameCall( strikeOtmCall ) );
    fLegSelected( m_rLegDefLong[1].dblSpread, strikeOtmPut,  citerChain->first, chain.GetIQFeedNamePut( strikeOtmPut ) );
  }
  else {
    fLegSelected( m_rLegDefShort[0].dblSpread, strikeOtmCall, citerChain->first, chain.GetIQFeedNameCall( strikeOtmCall ) );
    fLegSelected( m_rLegDefShort[1].dblSpread, strikeOtmPut,  citerChain->first, chain.GetIQFeedNamePut( strikeOtmPut ) );
  }
}

const std::string /* static */ Strangle::Name( const std::string& sUnderlying, const mapChains_t& chains, boost::gregorian::date date, double price, double slope ) {
  std::string sName( "strangle-" + sUnderlying );
  ChooseLegs(
    slope, chains, date, price,
    [&sName](double dblSpread, double strike, boost::gregorian::date date, const std::string& sIQFeedName ){
      sName
        += "-"
        +  ou::tf::Instrument::BuildDate( date.year(), date.month(), date.day() )
        +  "-"
        +  boost::lexical_cast<std::string>( strike );
    }
    );
  return sName;
}

// applicable when running a long strangle strategy, has negative dblPrice1 in OrderManager on short strangle
void Strangle::CloseItmLegForProfit( double price, EOrderSide defaultOrderSide, fBuildLeg_t&& f ) {
//  for ( Leg& leg: m_vLeg ) {
//    if ( leg.CloseItmForProfit( price ) ) {
//      EOptionSide side = leg.GetPosition()->GetInstrument()->GetOptionSide(); // assumes an option
//      f( m_pPortfolio->Id(), side, price,
//        [this,defaultOrderSide](pPosition_t pPosition, pChartDataView_t pChartDataView, EColour colour ){
//          AddPosition( pPosition, pChartDataView, colour );
//          for ( Leg& leg: m_vLeg ) {
//            if ( leg.GetPosition()->Id() == pPosition->Id() ) {
//              leg.PlaceOrder( defaultOrderSide, 1 ); // dblPrice1 is coming out negative
//              break;
//            }
//          }
//        } );
//    }
//  }
}

size_t /* static */ Strangle::LegCount() {
  return nLegs;
}

void Strangle::CheckStop( double price ) {

  double strikeUpper {};
  double strikeLower {};
  double dblConstructedValue {};

  switch ( m_state ) {
    case State::Watching:
    case State::Positions:
      for ( Leg& leg: m_vLeg ) {
        dblConstructedValue += leg.ConstructedValue();
        double strike = leg.GetPosition()->GetInstrument()->GetStrike();
        //multiplier = leg.GetPosition()->GetInstrument()->GetRow().nMultiplier;
        if ( 0.0 == strikeUpper ) {
          strikeUpper = strikeLower = strike;
        }
        else {
          if ( strike > strikeUpper ) strikeUpper = strike;
          else {
            if ( strike < strikeLower ) strikeLower = strike;
            //else std::cout << "don't know what happened" << std::endl;
          }
        }
      }

      double lower = strikeLower - dblConstructedValue;
      double upper = strikeUpper + dblConstructedValue;
      double profit = dblConstructedValue;

      bool bExit( false );
      if ( ( price > strikeLower ) && ( price < strikeUpper ) ) {
        //status = "expires";
      }
      else {
        if ( ( price > lower ) && ( price < upper ) ) {
          //status = "profit";
          if ( price <= strikeLower ) profit = price - lower;
          if ( price >= strikeUpper ) profit = upper - price;
        }
        else {
          //status = "loss";
          if ( price <= lower ) profit = price - lower;
          if ( price >= upper ) profit = upper - price;
          bExit = true;
        }
      }

      if ( bExit ) {
        ClosePositions();
      }
      break;
  }

}

double Strangle::GetNet( double price ) {
  double dblNet {};
  double dblConstructedValue {};
  double strikeUpper {};
  double strikeLower {};
  //double multiplier {};
  for ( Leg& leg: m_vLeg ) {
    dblNet += leg.GetNet( price ); // out: leg stats
    double dblLegConstructedValue = leg.ConstructedValue();
    std::cout << ",constructed@" << dblLegConstructedValue;
    dblConstructedValue += dblLegConstructedValue;
    std::cout << std::endl;
    double strike = leg.GetPosition()->GetInstrument()->GetStrike();
    //multiplier = leg.GetPosition()->GetInstrument()->GetRow().nMultiplier;
    if ( 0.0 == strikeUpper ) {
      strikeUpper = strikeLower = strike;
    }
    else {
      if ( strike > strikeUpper ) strikeUpper = strike;
      else {
        if ( strike < strikeLower ) strikeLower = strike;
        else std::cout << "don't know what happened" << std::endl;
      }
    }
  }

  //double adjustment = dblConstructedValue / multiplier;
  double adjustment = dblConstructedValue;
  double lower = strikeLower - adjustment;
  double upper = strikeUpper + adjustment;
  double profit = adjustment;
  double profitTotal {};

  std::string status;
  if ( ( price > strikeLower ) && ( price < strikeUpper ) ) {
    status = "expires";
  }
  else {
    if ( ( price > lower ) && ( price < upper ) ) {
      status = "profit";
      if ( price <= strikeLower ) profit = price - lower;
      if ( price >= strikeUpper ) profit = upper - price;
    }
    else {
      status = "loss";
      if ( price <= lower ) profit = price - lower;
      if ( price >= upper ) profit = upper - price;
    }
  }

  profitTotal += profit;

  std::cout
    << "  constructed: " << adjustment
    << ",lowerBE: " << lower
    << ",upperBE: " << upper
    << "=>" << status
    << "@" << profit
    << std::endl;
  return profitTotal;
}

} // namespace option
} // namespace tf
} // namespace ou
