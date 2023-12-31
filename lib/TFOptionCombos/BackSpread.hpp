/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    BackSpread.h
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created: 2023/06/29 12:59:51
 */

#pragma once

#include "ComboTraits.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // option
namespace spread { // spread
namespace back { // back

size_t LegCount();

void ChooseLegs( // throw Chain exceptions
  ComboTraits::EMarketDirection
, const mapChains_t& chains
, boost::gregorian::date
, const SpreadSpecs&
, double priceUnderlying
, const fLegSelected_t&&
);

void FillLegNote( size_t ix, ComboTraits::EMarketDirection, LegNote::values_t& );

std::string Name(
  ComboTraits::EMarketDirection
, const mapChains_t& chains
, boost::gregorian::date
, const SpreadSpecs&
, double price
, const std::string& sUnderlying
);

void AddLegOrder(
  const LegNote::Type
, pOrderCombo_t
, const ou::tf::OrderSide::EOrderSide
, uint32_t nOrderQuantity
, pPosition_t
);

void Bind( ComboTraits& traits );

} // namespace back
} // namespace spread
} // namespace option
} // namespace tf
} // namespace ou
