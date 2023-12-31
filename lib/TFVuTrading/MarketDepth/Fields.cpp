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
 * File:    Fields.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: December 12, 2022 12:50:57
 */

#include <wx/defs.h>

#include "Fields.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth
namespace rung {

  const vElement_t vElement = {
    { (int)EField::PL,         50, "P/L",     wxRIGHT,  Colours( EColour::LightCyan,     EColour::Black, EColour::Cyan ) }
  , { (int)EField::BuyCount,   45, "Ticks",   wxCENTER, Colours( EColour::LightSkyBlue,  EColour::Black, EColour::PaleGoldenrod ) }
  , { (int)EField::BuyVolume,  45, "BVol",    wxCENTER, Colours( EColour::LightSkyBlue,  EColour::Black, EColour::PaleGoldenrod ) }
  , { (int)EField::BidSize,    45, "BSize",   wxCENTER, Colours( EColour::LightSkyBlue,  EColour::Black, EColour::DodgerBlue    ) }
  , { (int)EField::BidOrder,   40, "Buy",     wxCENTER, Colours( EColour::LightYellow,   EColour::Black, EColour::Yellow        ) }
  , { (int)EField::Price,      65, "Price",   wxCENTER, Colours( EColour::LightSeaGreen, EColour::Black, EColour::LightYellow   ) }
  , { (int)EField::AskOrder,   40, "Sell",    wxCENTER, Colours( EColour::LightYellow,   EColour::Black, EColour::Yellow        ) }
  , { (int)EField::AskSize,    45, "ASize",   wxCENTER, Colours( EColour::LightPink,     EColour::Black, EColour::Magenta       ) }
  , { (int)EField::SellVolume, 45, "AVol",    wxCENTER, Colours( EColour::LightPink,     EColour::Black, EColour::PaleGoldenrod ) }
  , { (int)EField::SellCount,  45, "Ticks",   wxCENTER, Colours( EColour::LightPink,     EColour::Black, EColour::PaleGoldenrod ) }
  , { (int)EField::Ticks,      45, "Ticks",   wxCENTER, Colours( EColour::DimGray,       EColour::White, EColour::PaleGoldenrod ) } // count of trades
  , { (int)EField::Volume,     60, "Vol",     wxCENTER, Colours( EColour::DimGray,       EColour::White, EColour::PaleGoldenrod ) } // sum of volume
  , { (int)EField::Static,     80, "SttcInd", wxLEFT,   Colours( EColour::DimGray,       EColour::White, EColour::PaleGoldenrod ) } // static indicators - pivots, ...
  , { (int)EField::Dynamic,   100, "DynInd",  wxLEFT,   Colours( EColour::DimGray,       EColour::White, EColour::PaleGoldenrod ) } // dynamic indicators - ema, ...
  };

} // namespace rung
} // namespace market depth
} // namespace tf
} // namespace ou
