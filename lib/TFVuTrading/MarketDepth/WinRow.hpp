/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    WinRow.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created on April 4, 2022  12:14
 */

// a generic composition of WinRowElements for visual presentation of data

#pragma once

#include <memory>
#include <vector>
#include <string>

#include <OUCommon/Colour.h>

#include "Fields.hpp"

class wxPoint;
class wxWindow;
class wxWindowDestroyEvent;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

class WinRowElement;

class WinRow {
public:

  using EColour = ou::Colour::wx::EColour;

  WinRow( wxWindow* parent, const vElement_t&, const wxPoint& origin, int RowHeight, bool bIsHeader );
  ~WinRow();

  using pWinRow_t = std::shared_ptr<WinRow>;
  static pWinRow_t Construct( wxWindow* parent, const vElement_t&, const wxPoint& origin, int RowHeight, bool bIsHeader );

  static int RowWidth( const vElement_t& );

  WinRowElement* operator[]( int );

protected:
private:

  using vWinRowElement_t = std::vector<WinRowElement*>;
  vWinRowElement_t m_vWinRowElement;

  bool m_bParentIsAlive;

  void Clear();

  void OnDestroy( wxWindowDestroyEvent& event );
};

} // market depth
} // namespace tf
} // namespace ou
