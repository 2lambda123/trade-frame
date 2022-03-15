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
 * File:    ConfigParser.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/at
 * Created: March 15, 2022 12:56
 */

#include <map>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace ou {
namespace tf {
namespace config {

struct per_symbol_choices_t {

  size_t nPriceBins;
  double dblPriceUpper;
  double dblPriceLower;

  size_t nVolumeBins;
  size_t nVolumeUpper;
  size_t nVolumeLower;

};

struct choices_t {

  bool bStartSimulator;
  std::string sGroupDirectory;
  size_t ib_instance;

  size_t nTimeBins;

  std::string sTimeUpper; // string from file
  boost::posix_time::ptime dtUpper; // convert to ptime
  double dblTimeUpper; // convert to double for use in rdaf

  std::string sTimeLower;
  boost::posix_time::ptime dtLower;
  double dblTimeLower;

  using mapInstance_t = std::map<std::string,per_symbol_choices_t>; // std::string = symbol
  mapInstance_t mapInstance;

};

bool Load( const std::string&, choices_t& );

} // namespace config
} // namespace tf
} // namespace ou

/* config file format:

time_bins=3600
#          yyyymmddThhmmss
time_upper=20220314T000000
time_lower=20220315T000000

#group_directory=/app/AutoTrade/20220223 20:05:28.214488-1
sim_start=off

[SPY]
price_bins=200
price_upper=200
price_lower=400
volume_bins= 100
volume_upper=10000
volume_lower=0

*/