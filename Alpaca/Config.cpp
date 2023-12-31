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
 * File:    Config.cpp
 * Author:  raymond@burkholder.net
 * Project: Alpaca
 * Created: june 4, 2022 05:19
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {
  static const std::string sChoice_AlpacaKey( "alpaca_key" );
  static const std::string sChoice_AlpacaSecret( "alpaca_secret" );
  static const std::string sChoice_AlpacaDomain( "alpaca_domain" );

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, bool bRequired, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = std::move( vm[name].as<T>() );
      BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
    }
    else {
      if ( bRequired ) {
        BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << name << "='";
        bOk = false;
      }
    }
  return bOk;
  }
}

namespace config {

bool Load( const std::string& sFileName, Choices& choices ) {

  bool bOk( true );

  try {

    po::options_description config( "alpaca config" );
    config.add_options()
      ( sChoice_AlpacaKey.c_str(), po::value<std::string>( &choices.m_sAlpacaKey ), "alpaca key" )
      ( sChoice_AlpacaSecret.c_str(), po::value<std::string>( &choices.m_sAlpacaSecret ), "alpaca secret" )
      ( sChoice_AlpacaDomain.c_str(), po::value<std::string>( &choices.m_sAlpacaDomain ), "alpaca domain" )
      ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "alpaca config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<std::string>( sFileName, vm, sChoice_AlpacaKey, true, choices.m_sAlpacaKey );
      bOk &= parse<std::string>( sFileName, vm, sChoice_AlpacaSecret, true, choices.m_sAlpacaSecret );
      bOk &= parse<std::string>( sFileName, vm, sChoice_AlpacaDomain, true, choices.m_sAlpacaDomain );

    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config

