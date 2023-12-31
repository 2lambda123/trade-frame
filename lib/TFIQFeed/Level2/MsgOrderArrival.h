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
 * File:    MsgOrderArrival.h
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed/Level2
 * Created on October 16, 2021 20:02
 */

#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/spirit/include/qi.hpp>

#include <boost/phoenix/core.hpp>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data
namespace msg { // message
namespace OrderArrival {

using td_t = boost::posix_time::time_duration;

struct time_t {
  int32_t hours;
  int32_t minutes;
  int32_t seconds;
  int32_t fractional;
  time_t(): hours {}, minutes {}, seconds {}, fractional {} {}
  time_t( int32_t hours_, int32_t minutes_, int32_t seconds_, int32_t fractional_ )
  : hours( hours_ ), minutes( minutes_ ), seconds( seconds_ ), fractional( fractional_ ) {}
  td_t time() const { return td_t( hours, minutes, seconds, fractional ); }
};

using gd_t = boost::gregorian::date;

struct date_t {
  int32_t year;
  int32_t month;
  int32_t day;
  date_t(): year {}, month {}, day {} {}
  date_t( int32_t year_, int32_t month_, int32_t day_ )
  : year( year_ ), month( month_ ), day( day_ ) {}
  gd_t date() const { return gd_t( year, month, day ); }
};

using ptime = boost::posix_time::ptime;

struct decoded {
  char chMsgType;
  std::string sSymbolName;
  uint64_t nOrderId;
  union MMID {
    uint32_t id;
    char rch[ 4 ];
    MMID(): id {} {}
  } mmid;
  char chOrderSide;  // 'A' Sell, 'B' Buy
  double dblPrice;
  uint32_t nQuantity;
  uint64_t nPriority;
  uint8_t nPrecision;
  time_t time;
  date_t date;
  decoded(): nOrderId {}, nQuantity {}, nPriority {} {}
  ptime dt() const { return ptime( date.date(), time.time() ); }
};

} // namespace OrderArrival
} // namespace msg
} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou

namespace arrival_msg_t = ou::tf::iqfeed::l2::msg::OrderArrival;

BOOST_FUSION_ADAPT_STRUCT(
  arrival_msg_t::time_t,
  (int32_t, hours)
  (int32_t, minutes)
  (int32_t, seconds)
  (int32_t, fractional)
  )

BOOST_FUSION_ADAPT_STRUCT(
  arrival_msg_t::date_t,
  (int32_t, year)
  (int32_t, month)
  (int32_t, day)
  )

BOOST_FUSION_ADAPT_STRUCT(
  arrival_msg_t::decoded,
  (char, chMsgType)
  (std::string, sSymbolName)
  (uint64_t, nOrderId)
  (char, mmid.rch[0])
  (char, mmid.rch[1])
  (char, mmid.rch[2])
  (char, mmid.rch[3])
  (char, chOrderSide)
  (double, dblPrice)
  (uint32_t, nQuantity)
  (uint64_t, nPriority)
  (uint8_t, nPrecision)
  (arrival_msg_t::time_t, time)
  (arrival_msg_t::date_t, date)
  )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data
namespace msg { // message
namespace OrderArrival {

  namespace qi = boost::spirit::qi;

  template<typename Iterator>
  struct parser_decoded: qi::grammar<Iterator, decoded()> {

    parser_decoded(): parser_decoded::base_type( start ) {

      ruleMsgType %=
          qi::char_( '6' ) // Order Summary
        | qi::char_( '3' ) // Order Add
        | qi::char_( '4' ) // Order Update
        | qi::char_( '0' ) // Price Level Order
        ;

      ruleUint8  %= qi::ushort_;
      ruleUint32 %= qi::ulong_;
      ruleUint64 %= qi::ulong_long;
      ruleSymbolName %= *( qi::char_ - qi::char_( ',' ) );
      ruleMMID %= qi::char_ - qi::char_( ',' );

      ruleOrderSide %=
          qi::char_( 'A' ) // Sell
        | qi::char_( 'B' ) // Buy)
        ;

      rulePrice %= qi::double_;

      ruleTime %=
        (
          ruleUint32 > qi::lit( ':' ) // HH
        > ruleUint32 > qi::lit( ':' ) // mm
        > ruleUint32 > qi::lit( '.' ) // ss
        > ruleUint32                  // fractional
        )
        ;

      ruleDate %=
        (
          ruleUint32 > qi::lit( '-' ) // year
        > ruleUint32 > qi::lit( '-' ) // month
        > ruleUint32                  // day
        )
        ;

      start %=
            ruleMsgType >> qi::lit( ',' ) // cMsgType
        >>  ruleSymbolName >> qi::lit( ',' ) // sSymbolName
        >> -ruleUint64 >> qi::lit( ',' ) // nOrderId
        >> -ruleMMID >> -ruleMMID >> -ruleMMID >> -ruleMMID >> qi::lit( ',' ) // four character MMID
        >>  ruleOrderSide >> qi::lit( ',' ) // ruleOrderSide
        >>  rulePrice >> qi::lit( ',' ) // dblPrice
        >>  ruleUint32 >> qi::lit( ',' ) // nQuantity
        >> -ruleUint64 >> qi::lit( ',' ) // nPriority
        >>  ruleUint8 >> qi::lit( ',' ) // nPrecision
        >> -ruleTime >> qi::lit( ',' ) // when is time not present?
        >>  ruleDate
        >>  qi::lit( ',' )
        ;

    }

    qi::rule<Iterator, char()> ruleMsgType;
    qi::rule<Iterator, char()> ruleOrderSide;
    qi::rule<Iterator, char()> ruleMMID;
    qi::rule<Iterator, uint8_t()> ruleUint8;
    qi::rule<Iterator, uint32_t()> ruleUint32;
    qi::rule<Iterator, uint64_t()> ruleUint64;
    qi::rule<Iterator, double()> rulePrice;
    qi::rule<Iterator, std::string()> ruleSymbolName;
    qi::rule<Iterator, time_t()> ruleTime;
    qi::rule<Iterator, date_t()> ruleDate;

    qi::rule<Iterator, decoded()> start;

  };

  template <typename T>
  bool Decode( parser_decoded<T>& parser, decoded& out, T begin, T end ) {

    //static parser_decoded<T> parser;
    // parser_decoded<T> parser; // do we want to recreate this each time?

    // nasdaq l2: '6,QQQ,,AMEX,A,408.8400,100,,4,17:52:38.059128,2022-04-01,'
    bool bOk = parse( begin, end, parser, out );

    return bOk;
  }

} // namespace OrderArrival
} // namespace msg
} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
