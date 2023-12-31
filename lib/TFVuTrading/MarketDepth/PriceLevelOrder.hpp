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
 * File:    PriceLevelOrder.hpp (from AppDoM)
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading/MarketDepth
 * Created: 2022/11/27 12:41:04
 */

#pragma once

#include <TFTrading/Order.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

class PriceLevelOrder {
public:

  using pOrder_t = ou::tf::Order::pOrder_t;
  using fUpdateQuantity_t = std::function<void(uint32_t)>;
  using fExecution_t = std::function<void(const ou::tf::Execution&)>;

  PriceLevelOrder();
  PriceLevelOrder( pOrder_t pOrder );
  PriceLevelOrder( PriceLevelOrder&& rhs );
  PriceLevelOrder& operator=( pOrder_t pOrder );
  PriceLevelOrder& operator=( PriceLevelOrder&& rhs );

  ~PriceLevelOrder();

  pOrder_t Order() { return m_pOrder; }

  void Set( fUpdateQuantity_t&&, fExecution_t&& );

  void SetEvents();
  void ClearEvents();

  void HandleOnPartialFill( const ou::tf::Order& );
  void HandleOnOrderFilled( const ou::tf::Order& );
  void HandleOnOrderCancelled( const ou::tf::Order& ); // does order active go to 0?
  void HandleOnOrderExecution( const std::pair<const ou::tf::Order&, const ou::tf::Execution&>& );

protected:

  pOrder_t m_pOrder;

  fUpdateQuantity_t m_fUpdateQuantity;
  fExecution_t m_fExecution;

private:

}; // PriceLevelOrder

} // market depth
} // namespace tf
} // namespace ou
