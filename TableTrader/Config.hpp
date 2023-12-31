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
 * File:    Config.hpp
 * Author:  raymond@burkholder.net
 * Project: TableTrader
 * Created: 2022/08/02 15:15:19
 */

#pragma once

#include <string>
#include <vector>

namespace config {

struct Choices {

  // Interactive Brokers api instance
  int ib_client_id;
  unsigned int ib_client_port;

  // simple auth
  std::string m_sUIUserName;
  std::string m_sUIPassWord;

  using vCandidateFutures_t = std::vector<std::string>;
  vCandidateFutures_t m_vCandidateFutures;
};

bool Load( const std::string& sFileName, Choices& );

} // namespace config
