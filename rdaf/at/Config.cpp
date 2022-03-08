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
 * Project: rdaf/at
 * Created: March 7, 2022 14:35
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.h"

namespace {
  static const std::string sOption_Symbol( "symbol" );
  static const std::string sOption_PeriodWidth( "period_width" );
  static const std::string sOption_MA1Periods( "ma1_periods" );
  static const std::string sOption_MA2Periods( "ma2_periods" );
  static const std::string sOption_MA3Periods( "ma3_periods" );
  static const std::string sOption_GroupDirectory( "group_directory" );
  static const std::string sOption_SimStart( "sim_start" );
  static const std::string sOption_IbInstance( "ib_instance" );

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = vm[name].as<T>();
      BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << name << "='";
      bOk = false;
    }
  return bOk;
  }
}

namespace config {

bool Load( const std::string& sFileName, Options& options ) {

  bool bOk( true );

  try {

    po::options_description config( "AutoTrade Config" );
    config.add_options()
      ( sOption_Symbol.c_str(), po::value<std::string>( &options.sSymbol), "symbol" )

      ( sOption_PeriodWidth.c_str(), po::value<int>( &options.nPeriodWidth), "period width (sec)" )
      ( sOption_MA1Periods.c_str(),  po::value<int>( &options.nMA1Periods), "ma1 (#periods)" )
      ( sOption_MA2Periods.c_str(),  po::value<int>( &options.nMA2Periods), "ma2 (#periods)" )
      ( sOption_MA3Periods.c_str(),  po::value<int>( &options.nMA3Periods), "ma3 (#periods)" )

      ( sOption_GroupDirectory.c_str(), po::value<std::string>( &options.sGroupDirectory)->default_value( "" ), "hdf5 group directory" )

      ( sOption_SimStart.c_str(), po::value<bool>( &options.bSimStart)->default_value( false ), "auto start simulation" )
      ( sOption_IbInstance.c_str(), po::value<int>( &options.nIbInstance)->default_value( 1 ), "IB instance" )
      ;

    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "AutoTrade config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk |= parse<std::string>( sFileName, vm, sOption_Symbol, options.sSymbol );
      std::replace_if( options.sSymbol.begin(), options.sSymbol.end(), [](char ch)->bool{return '~' == ch;}, '#' );

      bOk |= parse<int>( sFileName, vm, sOption_PeriodWidth, options.nPeriodWidth );
      bOk |= parse<int>( sFileName, vm, sOption_MA1Periods,  options.nMA1Periods );
      bOk |= parse<int>( sFileName, vm, sOption_MA2Periods,  options.nMA2Periods );
      bOk |= parse<int>( sFileName, vm, sOption_MA3Periods,  options.nMA3Periods );

      // TODO: need to make this optional?
      bOk |= parse<std::string>( sFileName, vm, sOption_GroupDirectory, options.sGroupDirectory );

      bOk |= parse<bool>( sFileName, vm, sOption_SimStart, options.bSimStart );
      bOk |= parse<int>( sFileName, vm, sOption_IbInstance, options.nIbInstance );
    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config
