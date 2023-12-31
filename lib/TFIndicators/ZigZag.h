/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

// Bollinger page 91 suggests 0.17 * sqrt( price ) is a good filter width
// could set this number on each new peak

// 2012/03/31 http://www.olsenblog.com/wp-content/uploads/2009/11/conferencemanchester091004.pdf
// 2012/04/07 http://finance.martinsewell.com/stylized-facts/scaling/Guillaume-etal1997.pdf  3.7 Def 7 Direction Change Indicator, refines zigzag for risk profiles

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <TFTimeSeries/TimeSeries.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class ZigZag{
public:

  ZigZag();
  ZigZag( double FilterWidth );
  ~ZigZag();

  void SetFilterWidth( double width ) { m_dblFilterWidth = width; };
  double GetFilterWidth() const { return m_dblFilterWidth; };

  void Check( ptime dt, double val );

  enum EDirection { Init, Start, Down, Up };  // start, down, up are visible in OnPeakFoundHandler

  typedef FastDelegate4<const ZigZag&, ptime, double, EDirection> OnPeakFoundHandler;
  void SetOnPeakFound( OnPeakFoundHandler function ) {
    OnPeakFound = function;
  }

  typedef FastDelegate1<const ZigZag&> OnDecisionPointFoundHandler;
  void SetUpDecisionPointFound( OnDecisionPointFoundHandler function ) {
    UpDecisionPointFound = function;
  }
  void SetDnDecisionPointFound( OnDecisionPointFoundHandler function ) {
    DnDecisionPointFound = function;
  }

protected:
  double m_dblFilterWidth; // pt1 becomes new anchor when abs(pt0-pt1)>delta
  int m_cntNewUp, m_cntNewDown, m_cntTurns;
  EDirection m_PatternState;

private:
  double m_dblPatternPt0,  // pattern end point, drags pt1 away from anchor, but can retrace at will
    m_dblPatternPt1; // pattern mid point, can only move away from anchor point
  ptime m_dtPatternPt1;   // when it was last encountered
  OnPeakFoundHandler OnPeakFound;
  OnDecisionPointFoundHandler UpDecisionPointFound, DnDecisionPointFound;
};

// template sometime to handle Quote, CTrade, CPrice
class ZigZagTotalMovement: public ZigZag {
public:
  ZigZagTotalMovement( Quotes&, double );
  ~ZigZagTotalMovement();
  double Sum() const { return m_sum; };
protected:
private:
  double m_sum;
  double m_last;
  Quotes& m_quotes;
  void HandleQuote( const Quote& );
  void HandlePeakFound( const ZigZag&, ptime, double, ZigZag::EDirection );
};

} // namespace tf
} // namespace ou
