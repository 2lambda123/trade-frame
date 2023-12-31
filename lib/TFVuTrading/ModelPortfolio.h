/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once

#include <set>

#include <TFTrading/PortfolioManager.h>

#include "ModelBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// the model is designed for a table view

class ModelPortfolio: public ModelBase<ModelPortfolio> {
public:

  using PortfolioManager = ou::tf::PortfolioManager;
  using idPortfolio_t = PortfolioManager::idPortfolio_t;
  using pPortfolio_t = PortfolioManager::pPortfolio_t;

  struct DataViewItemPortfolio: public DataViewItem<pPortfolio_t::element_type> {
    DataViewItemPortfolio( shared_ptr& ptr )
      : DataViewItem<pPortfolio_t::element_type>( EMTPortfolio, ptr ) {};
    DataViewItemPortfolio( EModelType eModelType, shared_ptr& ptr )
      : DataViewItem<pPortfolio_t::element_type>( eModelType, ptr ) {};
    virtual void AssignFirstColumn( wxVariant& variant ) /* const */ {
      variant = GetPtr()->GetRow().idPortfolio;
    }
  };

  struct wxDataViewItem_Portfolio: public wxDataViewItem_typed<DataViewItemPortfolio> {};

  using setItems_t = std::set<DataViewItemPortfolio*>;

  ModelPortfolio(void);
  ~ModelPortfolio(void);

  void AddPortfolioToModel( DataViewItemPortfolio* );
  void ClearItems( void );

protected:
private:

//  typedef std::map<idPortfolio_t, DataViewItemPortfolio> mapItems_t;
  //typedef mapItems_t::const_iterator mapItems_citer_t;
  using setItems_citer_t = setItems_t::const_iterator;
  setItems_t m_setItems;

  // refactor this to the inherited class
  virtual bool IsContainer( const wxDataViewItem& item ) const;
  virtual wxDataViewItem GetParent( const wxDataViewItem& item ) const;
  virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const;
  virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const;

  PortfolioManager& m_mgrPortfolio;  // database must be open before processing portfolios

  void ProcessUpdatedItemDetails( DataViewItemPortfolio& item );

};

} // namespace tf
} // namespace ou
