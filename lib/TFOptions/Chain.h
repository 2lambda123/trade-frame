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
 * File:    Chains.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created: June 23, 2019, 9:49 PM
 */

#ifndef CHAIN_H
#define CHAIN_H

#include <cassert>
#include <iostream>
#include <algorithm>
#include <functional>

#include <map>
#include <string>
#include <stdexcept>

// use this for light weight strike calculations and name lookups

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

namespace chain {

  struct OptionName { // inherit to add addtiional fields
    std::string sIQFeedSymbolName;

    OptionName() {}
    virtual ~OptionName() {}
    OptionName( const std::string& sName_ )
    : sIQFeedSymbolName( sName_ ) {}
    OptionName( const std::string&& sName_ )
    : sIQFeedSymbolName( std::move( sName_ ) ) {}
    OptionName( const OptionName& rhs )
    : sIQFeedSymbolName( rhs.sIQFeedSymbolName ) {}
    OptionName( const OptionName&& rhs )
    : sIQFeedSymbolName( std::move( rhs.sIQFeedSymbolName ) ) {}
  };

  template<typename Option>
  struct Strike {
    Option call;
    Option put;
    Strike() {}
    Strike( Option&& call_, Option&& put_ )
    : call( std::move( call_ ) ),
      put( std::move( put_ ) )
      {}
    Strike( const Strike& rhs )
    : call( rhs.call ),
      put( rhs.put )
      {}
    Strike( const Strike&& rhs )
    : call( std::move( rhs.call ) ),
      put( std::move( rhs.put ) )
      {}
  };
}

template<typename Option>
class Chain {
public:

  using option_t = Option;
  using strike_t = chain::Strike<option_t>;

  using fStrike_t = std::function<void( double, const strike_t& )>;

  Chain() {}
  Chain( Chain&& rhs ) {
    m_mapChain = std::move( rhs.m_mapChain );
  }
  virtual ~Chain() {};

  struct exception_strike_not_found: public std::runtime_error {
    exception_strike_not_found( const char* ch ): std::runtime_error( ch ) {}
  };
  struct exception_at_start_of_chain: public exception_strike_not_found {
    exception_at_start_of_chain( const char* ch ): exception_strike_not_found( ch ) {}
  };

  Option& SetIQFeedNameCall( double strike, const std::string& sIQFeedSymbolName );
  Option& SetIQFeedNamePut(  double strike, const std::string& sIQFeedSymbolName );

  const std::string GetIQFeedNameCall( double strike ) const;
  const std::string GetIQFeedNamePut(  double strike ) const;

  void Erase( double strike );

  const chain::Strike<Option>& GetExistingStrike( double strike ) const;
  //const chain::Strike<Option>& GetStrike( double strike ) const;
  chain::Strike<Option>& GetStrike( double strike );

  double Put_Itm( double ) const ;
  double Put_ItmAtm( double ) const;
  double Put_Atm( double ) const;
  double Put_OtmAtm( double ) const;
  double Put_Otm( double ) const;

  double Call_Itm( double ) const;
  double Call_ItmAtm( double ) const;
  double Call_Atm( double ) const;
  double Call_OtmAtm( double ) const;
  double Call_Otm( double ) const;

  double Atm( double ) const;  // Call_Atm and Put_Atm seem identical

  // returns 0, 1, 2 strikes found
  // needs exact match on strikeSource
  int AdjacentStrikes( double strikeSource, double& strikeLower, double& strikeUpper ) const;

  void Strikes( fStrike_t&& fStrike ) const {
    for ( const typename mapChain_t::value_type& vt: m_mapChain ) {
      fStrike( vt.first, vt.second );
    }
  }

  using mapChain_t = std::map<double, strike_t>;

  typename mapChain_t::size_type Size() const { return m_mapChain.size(); }
  size_t EmitValues() const;
  size_t EmitSummary() const;

  void Test( double price );

protected:

  typename mapChain_t::const_iterator FindStrike( const double strike ) const;
  typename mapChain_t::iterator FindStrike( const double strike );

private:

  mapChain_t m_mapChain;

};

// methods:

template<typename Option>
double Chain<Option>::Put_Itm( double value ) const { // price < strike
  typename mapChain_t::const_iterator iter = std::upper_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](double value, const typename mapChain_t::value_type& vt)->bool{ return value < vt.first; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Put_Itm not found" );
  return iter->first;
}

template<typename Option>
double Chain<Option>::Put_ItmAtm( double value ) const { // price <= strike
  typename mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const typename mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Put_ItmAtm not found" );
  return iter->first;
}

template<typename Option>
double Chain<Option>::Put_Atm( double value ) const { // closest strike (use itm vs otm)
  double atm {};
  typename mapChain_t::const_iterator iter1 = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const typename mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter1 ) throw exception_strike_not_found( "Put_Atm not found" );
  if ( value == iter1->first ) {
    atm = value;
  }
  else {
    if ( m_mapChain.begin() == iter1 ) {
      atm = value;
    }
    else {
      typename mapChain_t::const_iterator iter2 = iter1;
      iter2--;
      if ( ( iter1->first - value ) < ( value - iter2->first ) ) {
        atm = iter1->first;
      }
      else {
        atm = iter2->first;
      }
    }
  }
  assert( 0.0 != atm );
  return atm;
}

template<typename Option>
double Chain<Option>::Put_OtmAtm( double value ) const { // price >= strike
  typename mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const typename mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Put_OtmAtm not found" );
  if ( value == iter->first ) {
    // atm
  }
  else {
    if ( m_mapChain.begin() == iter ) {
      throw exception_at_start_of_chain( "Put_OtmAtm at begin of chain" );
    }
    else {
      iter--; // strike will be OTM
    }
  }
  return iter->first;
}

template<typename Option>
double Chain<Option>::Put_Otm( double value ) const { // price > strike
  typename mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const typename mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Put_Otm not found" );
  if ( m_mapChain.begin() == iter ) {
    throw exception_at_start_of_chain( "Put_Otm at begin of chain" );
  }
  else {
    iter--; // strike will be OTM
  }
  return iter->first;
}

template<typename Option>
double Chain<Option>::Call_Itm( double value ) const { // price > strike
  typename mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const typename mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Call_Itm not found" );
    if ( m_mapChain.begin() == iter ) {
      throw exception_at_start_of_chain( "Call_Itm at begin of chain" );
    }
    else {
      iter--;
    }
  return iter->first;
}

template<typename Option>
double Chain<Option>::Call_ItmAtm( double value ) const { // price >= strike
  typename mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const typename mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Call_ItmAtm not found" );
  if ( value == iter->first ) {
    // atm
  }
  else {
    if ( m_mapChain.begin() == iter ) {
      throw exception_at_start_of_chain( "Call_ItmAtm at begin of chain" );
    }
    else {
      iter--; // strike will be Itm
    }
  }
  return iter->first;
}

template<typename Option>
double Chain<Option>::Call_Atm( double value ) const { // closest strike (use itm vs otm)
  double atm {};
  typename mapChain_t::const_iterator iter1 = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const typename mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter1 ) throw exception_strike_not_found( "Call_Atm not found" );
  if ( value == iter1->first ) {
    atm = value;
  }
  else {
    if ( m_mapChain.begin() == iter1 ) {
      atm = value;
    }
    else {
      typename mapChain_t::const_iterator iter2 = iter1;
      iter2--;
      if ( ( iter1->first - value ) < ( value - iter2->first ) ) {
        atm = iter1->first;
      }
      else {
        atm = iter2->first;
      }
    }
  }
  assert( 0.0 != atm );
  return atm;
}

template<typename Option>
double Chain<Option>::Call_OtmAtm( double value ) const { // price <= strike
  typename mapChain_t::const_iterator iter = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const typename mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Call_OtmAtm not found" );
  return iter->first;
}

template<typename Option>
double Chain<Option>::Call_Otm( double value ) const { // price < strike
  typename mapChain_t::const_iterator iter = std::upper_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](double value, const typename mapChain_t::value_type& vt)->bool{ return value < vt.first; } );
  if ( m_mapChain.end() == iter ) throw exception_strike_not_found( "Call_Otm not found" );
  return iter->first;
}

template<typename Option>
double Chain<Option>::Atm( double value ) const { // closest strike (use itm vs otm)
  double atm {};
  typename mapChain_t::const_iterator iter1 = std::lower_bound(
    m_mapChain.begin(), m_mapChain.end(), value,
    [](const typename mapChain_t::value_type& vt, double value)->bool{ return vt.first < value; } );
  if ( m_mapChain.end() == iter1 ) throw exception_strike_not_found( "Call_Atm not found" );
  if ( value == iter1->first ) {
    atm = value;
  }
  else {
    if ( m_mapChain.begin() == iter1 ) {
      atm = value;
    }
    else {
      typename mapChain_t::const_iterator iter2 = iter1;
      iter2--;
      if ( ( iter1->first - value ) < ( value - iter2->first ) ) {
        atm = iter1->first;
      }
      else {
        atm = iter2->first;
      }
    }
  }
  assert( 0.0 != atm );
  return atm;
}

template<typename Option>
int Chain<Option>::AdjacentStrikes( double strikeSource, double& strikeLower, double& strikeUpper ) const {
  strikeLower = strikeUpper = 0.0;
  int nReturn {};
  typename mapChain_t::const_iterator iterSource = m_mapChain.find( strikeSource );
  if ( m_mapChain.end() != iterSource ) {
    typename mapChain_t::const_iterator iterLower = iterSource;
    if ( m_mapChain.begin() != iterLower ) {
      iterLower--;
      strikeLower = iterLower->first;
      nReturn++;
    }
    typename mapChain_t::const_iterator iterUpper = iterSource;
    iterUpper++;
    if ( m_mapChain.end() != iterUpper ) {
      strikeUpper = iterUpper->first;
      nReturn++;
    }
  }
  return nReturn;
}

template<typename Option>
Option& Chain<Option>::SetIQFeedNameCall( double dblStrike, const std::string& sIQFeedSymbolName ) {
  typename mapChain_t::iterator iter = m_mapChain.find( dblStrike );
  if ( m_mapChain.end() == iter ) {
    auto result = m_mapChain.emplace( typename mapChain_t::value_type( dblStrike, strike_t() ) );
    assert( result.second );
    iter = result.first;
  }
  if ( iter->second.call.sIQFeedSymbolName.empty() ) {
    iter->second.call.sIQFeedSymbolName = sIQFeedSymbolName;
  }
  else {
    std::cout
      << "Chain<Option>::SetIQFeedNameCall duplicate existing: "
      << iter->second.call.sIQFeedSymbolName
      << ", new "
      << sIQFeedSymbolName
      << ", skipped"
      << std::endl;
    throw std::runtime_error( "duplicate call" );
    // maybe throw an exception and let caller handle it: ignore or not
    //assert( iter->second.call.sIQFeedSymbolName == sIQFeedSymbolName );
  }
  return iter->second.call;
}

template<typename Option>
Option& Chain<Option>::SetIQFeedNamePut( double dblStrike, const std::string& sIQFeedSymbolName ) {
  typename mapChain_t::iterator iter = m_mapChain.find( dblStrike );
  if ( m_mapChain.end() == iter ) {
    auto result = m_mapChain.emplace( typename mapChain_t::value_type( dblStrike, strike_t() ) );
    assert( result.second );
    iter = result.first;
  }
  if ( iter->second.put.sIQFeedSymbolName.empty() ) {
    iter->second.put.sIQFeedSymbolName = sIQFeedSymbolName;
  }
  else {
    std::cout
      << "Chain<Option>::SetIQFeedNamePut duplicate existing: "
      << iter->second.put.sIQFeedSymbolName
      << ", new "
      << sIQFeedSymbolName
      << ", skipped"
      << std::endl;
    throw std::runtime_error( "duplicate put" );
    // maybe throw an exception and let caller handle it: ignore or not
    //assert( iter->second.put.sIQFeedSymbolName == sIQFeedSymbolName );
  }
  return iter->second.put;
}

template<typename Option>
const std::string Chain<Option>::GetIQFeedNameCall( double dblStrike ) const {
  typename mapChain_t::const_iterator iter = FindStrike( dblStrike );
  return iter->second.call.sIQFeedSymbolName;
}

template<typename Option>
const std::string Chain<Option>::GetIQFeedNamePut( double dblStrike ) const {
  typename mapChain_t::const_iterator iter = FindStrike( dblStrike );
  return iter->second.put.sIQFeedSymbolName;
}

template<typename Option>
void Chain<Option>::Erase( double dblStrike ) {
  typename mapChain_t::const_iterator iter = FindStrike( dblStrike );
  m_mapChain.erase( iter );
}

template<typename Option>
const chain::Strike<Option>& Chain<Option>::GetExistingStrike( double dblStrike ) const { // this one doesn't make much sense
  typename mapChain_t::const_iterator citer = m_mapChain.find( dblStrike );
  if ( m_mapChain.end() == citer ) {
    throw exception_strike_not_found( "Chain::GetExistingStrike const: no strike" );
  }
  return citer->second;
}

//template<typename Option>
//const chain::Strike<Option>& Chain<Option>::GetStrike( double dblStrike ) const { // this one doesn't make much sense
//  typename mapChain_t::iterator iter = m_mapChain.find( dblStrike );
//  if ( m_mapChain.end() == iter ) {
//    iter = m_mapChain.insert( m_mapChain.begin(), std::move( typename mapChain_t::value_type( dblStrike, strike_t() ) ) );
//  }
//  return iter->second;
//}

template<typename Option>
chain::Strike<Option>& Chain<Option>::GetStrike( double dblStrike ) {
  typename mapChain_t::iterator iter = m_mapChain.find( dblStrike );
  if ( m_mapChain.end() == iter ) {
    auto result = m_mapChain.emplace( typename mapChain_t::value_type( dblStrike, strike_t() ) );
    assert( result.second );
    iter = result.first;
  }
  return iter->second;
}

// const iterator
template<typename Option>
typename Chain<Option>::mapChain_t::const_iterator Chain<Option>::FindStrike( const double strike ) const {
  typename mapChain_t::const_iterator iter = m_mapChain.find( strike );
  if ( m_mapChain.end() == iter ) {
    throw exception_strike_not_found( "Chain::FindStrike const: no strike" );
  }
  return iter;
}

// regular iterator
template<typename Option>
typename Chain<Option>::mapChain_t::iterator Chain<Option>::FindStrike( const double strike ) {
  typename mapChain_t::iterator iter = m_mapChain.find( strike );
  if ( m_mapChain.end() == iter ) {
    std::cout
      << "Chain::FindStrike error: "
      << "strike " << strike
      << ", m_mapChain size=" << m_mapChain.size()
      << std::endl;
    throw exception_strike_not_found( "Chain::FindStrike: no strike" );
  }
  return iter;
}

template<typename Option>
size_t Chain<Option>::EmitValues() const { // TODO: supply output stream
  size_t cnt {};
  std::for_each( m_mapChain.begin(), m_mapChain.end(), [&cnt](const typename mapChain_t::value_type& vt){
    cnt++;
    std::cout
      << vt.first << ": "
      << vt.second.call.sIQFeedSymbolName
      << ", "
      << vt.second.put.sIQFeedSymbolName
      << std::endl;
  });
  return cnt;
}

template<typename Option>
size_t Chain<Option>::EmitSummary() const { // TODO: supply output stream
  size_t nStrikes {};
  size_t nCalls {};
  size_t nPuts {};
  std::for_each( m_mapChain.begin(), m_mapChain.end(), [ &nStrikes, &nCalls, &nPuts](const typename mapChain_t::value_type& vt){
    nStrikes++;
    if ( 0 != vt.second.call.sIQFeedSymbolName.size() ) nCalls++;
    if ( 0 != vt.second.put.sIQFeedSymbolName.size() ) nPuts++;
  });
    std::cout
      << "  #strikes=" << nStrikes
      << ", #calls=" << nCalls
      << ", #puts=" << nPuts
      << std::endl;
  return ( nCalls + nPuts );
}

template<typename Option>
void Chain<Option>::Test( double price ) { // TODO: supply output stream
  try {
    std::cout << "Put_Itm: "     << Put_Itm( price ) << std::endl;
    std::cout << "Put_ItmAtm: "  << Put_ItmAtm( price ) << std::endl;
    std::cout << "Put_Atm: "     << Put_Atm( price ) << std::endl;
    std::cout << "Put_OtmAtm: "  << Put_OtmAtm( price ) << std::endl;
    std::cout << "Put_Otm: "     << Put_Otm( price ) << std::endl;

    std::cout << "Call_Itm: "    << Call_Itm( price ) << std::endl;
    std::cout << "Call_ItmAtm: " << Call_ItmAtm( price ) << std::endl;
    std::cout << "Call_Atm: "    << Call_Atm( price ) << std::endl;
    std::cout << "Call_OtmAtm: " << Call_OtmAtm( price ) << std::endl;
    std::cout << "Call_Otm: "    << Call_Otm( price ) << std::endl;
  }
  catch ( std::runtime_error& e ) {
    std::cout << "Chain::Test runtime error: " << e.what() << std::endl;
  }
}

} // namespace option
} // namespace tf
} // namespace ou

#endif /* CHAIN_H */

