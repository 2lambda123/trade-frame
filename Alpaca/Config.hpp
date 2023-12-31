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
 * Project: Alpaca
 * Created: june 4, 2022 05:19
  */

#pragma once

#include <string>

namespace config {

struct Choices {

  std::string m_sAlpacaKey;
  std::string m_sAlpacaSecret;
  std::string m_sAlpacaDomain; // paper-api.alpaca.markets or api.alpaca.markets
};

bool Load( const std::string& sFileName, Choices& );

} // namespace config
