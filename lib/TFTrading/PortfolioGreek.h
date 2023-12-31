/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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
 * File:   PortfolioGreek.h
 * Author: raymond@burkholder.net
 *
 * Created on July 31, 2018, 11:16 AM
 */

#ifndef PORTFOLIOGREEK_H
#define PORTFOLIOGREEK_H

#include "Portfolio.h"
#include "PositionGreek.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class PortfolioGreek: public Portfolio {
  friend class boost::serialization::access;
public:

  friend std::ostream& operator<<( std::ostream& os, const Portfolio& );

  using idPosition_t = keytypes::idPosition_t;
  using idPortfolio_t = keytypes::idPortfolio_t;

  using pPositionGreek_t = PositionGreek::pPositionGreek_t;

  using pPortfolioGreek_t = std::shared_ptr<PortfolioGreek>;

  PortfolioGreek(
    const idPortfolio_t& idPortfolio, const idAccountOwner_t& idAccountOwner, const idPortfolio_t& idOwner, EPortfolioType ePortfolioType_,
    currency_t eCurrency, const std::string& sDescription );
  virtual ~PortfolioGreek( );

  pPositionGreek_t AddPosition( const std::string& sName, pPositionGreek_t );
  void DeletePosition( const std::string& sName, pPositionGreek_t ); // supply the position to test against the name or ptr value (ptr not used here)

  void AddSubPortfolio( pPortfolioGreek_t& );
  void RemoveSubPortfolio( const idPortfolio_t& idPortfolio );

protected:
private:

};

std::ostream& operator<<( std::ostream& os, const PortfolioGreek& );

} // namespace tf
} // namespace ou

#endif /* PORTFOLIOGREEK_H */

