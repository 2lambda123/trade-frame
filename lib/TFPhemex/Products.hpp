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
 * File:    Products.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFPhemex
 * Created: July 26, 2022 14:52:52
 */

#pragma once

#include <string>
#include <vector>

namespace boost {
namespace json {
  class array;
  class value;
}
}

namespace json = boost::json;

namespace ou {
namespace tf {
namespace phemex {
namespace products {

// == Currency ==

struct Currency {
  std::string currency;
  std::string name;
  unsigned int code;
  unsigned int valueScale;
  uint64_t minValueEv;
  uint64_t maxValueEv;
  unsigned int needAddrTag;
  std::string status;
};

using vCurrency_t = std::vector<Currency>; // might need a map instead

void Decode( json::array const&, vCurrency_t& );

// == Product ==

struct Product {
  std::string symbol;
  unsigned int code;
  std::string type;
  std::string displaySymbol;
  std::string indexSymbol;
  std::string markSymbol;
  std::string fundingRateSymbol;
  std::string fundingRate8hSymbol;
  std::string contractUnderlyingAssets;
  std::string settleCurrency;
  std::string quoteCurrency;
  std::string baseCurrency;
  double contractSize;
  unsigned int lotSize;
  double tickSize;
  std::string baseTickSize;
  uint64_t baseTickSizeEv;
  std::string quoteTickSize;
  uint64_t quoteTickSizeEv;
  unsigned int priceScale;
  unsigned int ratioScale;
  unsigned int pricePrecision;
  unsigned int baseQtyPrecision;
  unsigned int quoteQtyPrecision;
  uint64_t minPriceEp;
  uint64_t maxPriceEp;
  uint64_t maxOrderQty;
  std::string minOrderValue;
  uint64_t minOrderValueEv;
  std::string maxBaseOrderSize;
  uint64_t maxBaseOrderSizeEv;
  std::string maxOrderValue;
  uint64_t maxOrderValueEv;
  std::string description;
  std::string status;
  uint64_t tipOrderQty;
  std::string defaultTakerFee;
  unsigned int defaultTakerFeeEr;
  std::string defaultMakerFee;
  unsigned int defaultMakerFeeEr;
  uint64_t listTime;
  uint64_t ieoOpenPriceEp;
  unsigned int ieoInitDurationMs;
  unsigned int buyPriceUpperLimitPct;
  unsigned int sellPriceLowerLimitPct;
};

using vProduct_t = std::vector<Product>; // minght use a map instead
void Decode( json::array const&, vProduct_t& );

// == riskLimits ==

struct riskLimits_detail;
using vRiskLimits_detail_t = std::vector<riskLimits_detail>;

struct riskLimits {
  std::string symbol;
  std::string steps;
  vRiskLimits_detail_t vRiskLimits_detail;
  riskLimits( riskLimits&& rhs )
  : symbol( std::move( rhs.symbol ) )
  , steps( std::move( rhs.steps ) )
  , vRiskLimits_detail( std::move( rhs.vRiskLimits_detail ) )
  {}
  riskLimits() {}
};

using vriskLimits_t = std::vector<riskLimits>;

struct riskLimits_detail {
  uint64_t limit;
  std::string initialMargin;  // "1.0%"
  uint64_t initialMarginEr;
  std::string maintenanceMargin; // "0.5%"
  uint64_t maintenanceMarginEr;
};

void Decode( json::array const&, vriskLimits_t& );

// == Leverages ==

struct Leverages {
  std::string initialMargin;
  uint64_t initialMarginEr;
  std::vector<unsigned int> options;
};

using vLeverages_t = std::vector<Leverages>;

void Decode( json::array const&, vLeverages_t& );

} // namespace products
} // namespace phemex
} // namespace tf
} // namespace ou
