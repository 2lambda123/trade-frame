/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/07/12

// This parser is used for OPTION and FOPTION symbol parsing

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/qi_repeat.hpp>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/stl.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

struct structParsedOptionSymbol1 {
  std::string sText;
  std::string sDigits;
  std::string sCode;
  double dblStrike;
  structParsedOptionSymbol1( void ) : dblStrike( 0.0 ) {};
};

struct structParsedOptionSymbol2 {
  boost::uint16_t nYear;
  boost::uint8_t nDay;
  structParsedOptionSymbol2( void ) : nYear( 0 ), nDay( 0 ) {};
};

struct structParsedOptionSymbol3 {
  std::string sText;
  std::string sMonth;
  boost::uint16_t nYear;
  std::string sCode;
  double dblStrike;
  structParsedOptionSymbol3( void ) : dblStrike( 0.0 ), nYear( 0 ) {};
};

} // namespace iqfeed
} // namespace tf
} // namespace ou

using pos1_t = ou::tf::iqfeed::structParsedOptionSymbol1;

BOOST_FUSION_ADAPT_STRUCT(
  pos1_t,
  (std::string, sText)
  (std::string, sDigits)
  (std::string, sCode)
  (double, dblStrike)
  )

using pos2_t = ou::tf::iqfeed::structParsedOptionSymbol2;

BOOST_FUSION_ADAPT_STRUCT(
  pos2_t,
  (boost::uint16_t, nYear)
  (boost::uint8_t, nDay)
  )

using pos3_t = ou::tf::iqfeed::structParsedOptionSymbol3; // Future Options

BOOST_FUSION_ADAPT_STRUCT(
  pos3_t,
  (std::string, sText)
  (std::string, sMonth)
  (boost::uint16_t, nYear)
  (std::string, sCode)
  (double, dblStrike)
  )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;
namespace ascii = boost::spirit::ascii;

using uint2_p = qi::uint_parser<unsigned, 10, 2, 2>;

template<typename Iterator>
struct OptionSymbolParser1: qi::grammar<Iterator, pos1_t()> {

  OptionSymbolParser1( void ): OptionSymbolParser1::base_type(start) {

    // define option processing rules
    ruleText %= +qi::char_( "A-Z" );
    ruleDigits %= +qi::char_( "0-9" );
    ruleCode %= qi::char_( "A-X" );
    ruleStrike %= qi::float_;

    start %= ruleText >> ruleDigits >> ruleCode >> ruleStrike;
  }

  qi::rule<Iterator, std::string()> ruleText;  // symbol
  qi::rule<Iterator, std::string()> ruleDigits;  // yydd
  qi::rule<Iterator, std::string()> ruleCode;  // month and call/put
  qi::rule<Iterator, double()> ruleStrike; // strike

  qi::rule<Iterator, pos1_t()> start;

};

template<typename Iterator>
struct OptionSymbolParser2: qi::grammar<Iterator, pos2_t()> {

  OptionSymbolParser2( void ): OptionSymbolParser2::base_type(start) {

    // define option processing rules
    ruleYear %= uint2_p();
    ruleDay  %= uint2_p();

    start %= ruleYear >> ruleDay;
  }

  qi::rule<Iterator, boost::uint16_t()> ruleYear;
  qi::rule<Iterator, boost::uint8_t()> ruleDay;

  qi::rule<Iterator, pos2_t()> start;

};

template<typename Iterator>
struct FOptionSymbolParser1: qi::grammar<Iterator, pos1_t()> {

  FOptionSymbolParser1( void ): FOptionSymbolParser1::base_type(start) {

    // define option processing rules
    ruleText %= +qi::char_( "@A-Z" );
    ruleDigits %= +qi::char_( "0-9" );
    ruleCode %= qi::char_( "CP" );
    ruleStrike %= qi::float_;

    start %= ruleText >> ruleDigits >> ruleCode >> ruleStrike;
  }

  qi::rule<Iterator, std::string()> ruleText;  // symbol
  qi::rule<Iterator, std::string()> ruleDigits;  // yydd
  qi::rule<Iterator, std::string()> ruleCode;  // month and call/put
  qi::rule<Iterator, double()> ruleStrike; // strike

  qi::rule<Iterator, pos1_t()> start;

};

template<typename Iterator>
struct FOptionSymbolParser3: qi::grammar<Iterator, pos3_t()> {

  FOptionSymbolParser3( void ): FOptionSymbolParser3::base_type(start) {

    // define option processing rules
    ruleMonth %= qi::char_( "FGHJKMNQUVXZ" );

    ruleSymbol0 %= qi::char_( "0-9A-Z" );
    ruleSymbol4 %= ruleSymbol0 >> ruleSymbol0 >> ruleSymbol0 >> ruleSymbol0;
    ruleSymbol3 %= ruleSymbol0 >> ruleSymbol0 >> ruleSymbol0;
    ruleSymbol2 %= ruleSymbol0 >> ruleSymbol0;
    ruleSymbol1 %= ruleSymbol0;
    ruleSymbol7 %= qi::char_( "@" ) >> ruleSymbol0 >> ruleSymbol0 >> ruleSymbol0;
    ruleSymbol6 %= qi::char_( "@" ) >> ruleSymbol0 >> ruleSymbol0;
    ruleSymbol5 %= qi::char_( "@" ) >> ruleSymbol0;

    ruleYear %= uint2_p();
    ruleOption %= qi::char_( "CP" );
    ruleStrike %= qi::float_;

    start %= (
             qi::hold[ ( ruleSymbol1 >> &( ruleMonth >> ruleYear ) ) ] // & is non-consuming lookahead
           | qi::hold[ ( ruleSymbol2 >> &( ruleMonth >> ruleYear ) ) ]
           | qi::hold[ ( ruleSymbol3 >> &( ruleMonth >> ruleYear ) ) ]
           | qi::hold[ ( ruleSymbol4 >> &( ruleMonth >> ruleYear ) ) ]
           | qi::hold[ ( ruleSymbol5 >> &( ruleMonth >> ruleYear ) ) ]
           | qi::hold[ ( ruleSymbol6 >> &( ruleMonth >> ruleYear ) ) ]
           | qi::hold[ ( ruleSymbol7 >> &( ruleMonth >> ruleYear ) ) ]
           )
      >> ruleMonth >> ruleYear>> ruleOption >> ruleStrike;
  }

  qi::rule<Iterator, std::string()> ruleSymbol0;
  qi::rule<Iterator, std::string()> ruleSymbol1;
  qi::rule<Iterator, std::string()> ruleSymbol2;
  qi::rule<Iterator, std::string()> ruleSymbol3;
  qi::rule<Iterator, std::string()> ruleSymbol4;
  qi::rule<Iterator, std::string()> ruleSymbol5;
  qi::rule<Iterator, std::string()> ruleSymbol6;
  qi::rule<Iterator, std::string()> ruleSymbol7;

  qi::rule<Iterator, std::string()> ruleMonth;  // month
  qi::rule<Iterator, boost::uint16_t()> ruleYear;  // yy
  qi::rule<Iterator, std::string()> ruleOption;  // call/put
  qi::rule<Iterator, double()> ruleStrike; // strike

  qi::rule<Iterator, pos3_t()> start;

};

} // namespace iqfeed
} // namespace tf
} // namespace ou
