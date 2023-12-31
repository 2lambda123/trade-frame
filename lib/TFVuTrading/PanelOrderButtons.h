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
 * File:    PanelOrderButtons.h
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading
 * Created: February 11, 2022 13:20
 */

#pragma once

#include <functional>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/panel.h>

#include "PanelOrderButtons_structs.h"

class wxButton;
class wxListCtrl;
class wxRadioButton;
class wxCheckBox;
class wxTextCtrl;
class wxRadioBox;
class wxListEvent;
class wxStaticText;

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANELORDERBUTTONS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELORDERBUTTONS_TITLE _("Order Buttons")
#define SYMBOL_PANELORDERBUTTONS_IDNAME ID_PanelOrderButtons
#define SYMBOL_PANELORDERBUTTONS_SIZE wxSize(400, 300)
#define SYMBOL_PANELORDERBUTTONS_POSITION wxDefaultPosition

class PanelOrderButtons: public wxPanel {
  friend class boost::serialization::access;
public:

  PanelOrderButtons();
  PanelOrderButtons(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANELORDERBUTTONS_IDNAME,
   const wxPoint& pos = SYMBOL_PANELORDERBUTTONS_POSITION,
   const wxSize& size = SYMBOL_PANELORDERBUTTONS_SIZE,
   long style = SYMBOL_PANELORDERBUTTONS_STYLE
   );
  virtual ~PanelOrderButtons();

  bool Create(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANELORDERBUTTONS_IDNAME,
   const wxPoint& pos = SYMBOL_PANELORDERBUTTONS_POSITION,
   const wxSize& size = SYMBOL_PANELORDERBUTTONS_SIZE,
   long style = SYMBOL_PANELORDERBUTTONS_STYLE
   );

   using fBtnDone_t = std::function<void()>; // undo state set for the button while 'latched'
   using fBtnOrder_t = std::function<void( const PanelOrderButtons_Order&, fBtnDone_t&& )>;

   void Set(
     fBtnOrder_t&&, // Buy
     fBtnOrder_t&&, // Sell
     fBtnOrder_t&&, // Close
     fBtnOrder_t&&  // Cancel
   );

   void Trigger( PanelOrderButtons_Order::EOrderMethod );

   void SetPriceAtFocus( const std::string& );

   void Update( const PanelOrderButtons_MarketData& );
   void Update( const PanelOrderButtons_PositionData& );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST
  , ID_PanelOrderButtons
  , ID_TXT_Symbol, ID_TXT_Quantity, ID_TXT_ProfitLoss
  , ID_BtnBuy, ID_BtnSell, ID_BtnClose, ID_BtnCancel
  , ID_CB_PositionEntry
  , ID_CB_PositionExitProfit
  , ID_CB_PositionExitStop
  , ID_TXT_QuanStock, ID_TXT_QuanFuture, ID_TXT_QuanOption
  , ID_TXT_PositionEntry
  , ID_TXT_PositionExitProfit
  , ID_TXT_PositionExitStop
  , ID_RADIO_PositionEntry
  , ID_RADIO_PositionExitProfit
  , ID_RADIO_PositionExitTop
  , ID_RADIO_Instrument
  , id_radioBase
  , id_radioSynthLong, id_radioCallItm, id_radioPutOtm
  , id_radioSynthShort, id_radioPutItm, id_radioCallOtm
  , ID_TXT_Base, ID_TXT_BaseAsk, ID_TXT_BaseBid
  , ID_TXT_Call1, ID_TXT_Call1Ask, ID_TXT_Call1Bid
  , ID_TXT_Put1, ID_TXT_Put1Ask, ID_TXT_Put1Bid
  , ID_TXT_Call2, ID_TXT_Call2Ask, ID_TXT_Call2Bid
  , ID_TXT_Put2, ID_TXT_Put2Ask, ID_TXT_Put2Bid
  , ID_CB_Stoch1, ID_CB_Stoch2, ID_CB_Stoch3
  };

  enum EFocus { None, Price, Profit, Stop } m_eFocus;

  PanelOrderButtons_Order m_order;

    wxTextCtrl* m_txtSymbol;
    wxTextCtrl* m_txtQuantity;
    wxTextCtrl* m_txtProfitLoss;

    wxTextCtrl* m_txtQuanStock;
    wxTextCtrl* m_txtQuanFuture;
    wxTextCtrl* m_txtQuanOption;
    wxCheckBox* m_cbEnablePositionEntry;
    wxTextCtrl* m_txtPricePositionEntry;
    wxRadioBox* m_radioPositionEntry;
    wxCheckBox* m_cbEnableProfitExit;
    wxTextCtrl* m_txtPriceProfitExit;
    wxRadioBox* m_radioExitProfit;
    wxCheckBox* m_cbEnableStopExit;
    wxTextCtrl* m_txtPriceStopExit;
    wxRadioBox* m_radioExitStop;

    wxButton* m_btnBuy;
    wxButton* m_btnSell;
    wxButton* m_btnClose;
    wxButton* m_btnCancel;

    wxRadioButton* m_radioBase;
    wxRadioButton* m_radioSynthLong;
    wxRadioButton* m_radioCallItm;
    wxRadioButton* m_radioPutOtm;
    wxRadioButton* m_radioSynthShort;
    wxRadioButton* m_radioPutItm;
    wxRadioButton* m_radioCallOtm;

    wxStaticText* m_txtBase;
    wxStaticText* m_txtBaseAsk;
    wxStaticText* m_txtBaseBid;
    wxStaticText* m_txtCallItmAsk;
    wxStaticText* m_txtCallItmBid;
    wxStaticText* m_txtPutOtmAsk;
    wxStaticText* m_txtPutOtmBid;
    wxStaticText* m_txtCallOtmAsk;
    wxStaticText* m_txtCallOtmBid;
    wxStaticText* m_txtPutItmAsk;
    wxStaticText* m_txtPutItmBid;

    wxCheckBox* m_cbEnableStoch1;
    wxCheckBox* m_cbEnableStoch2;
    wxCheckBox* m_cbEnableStoch3;

  fBtnOrder_t m_fBtnOrderBuy;
  fBtnOrder_t m_fBtnOrderSell;
  fBtnOrder_t m_fBtnOrderClose;
  fBtnOrder_t m_fBtnOrderCancel;

  void Init();
  void CreateControls();
  bool ShowToolTips() { return true; };

  bool ValidateFields();

  void ClickBuy();
  void ClickSell();

  void OnFocusChange( wxFocusEvent& event );

  void OnDestroy( wxWindowDestroyEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TXT_QuanStock
    void OnTXTQuanStockTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TXT_QuanFuture
    void OnTXTQuanFutureTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TXT_QuanOption
    void OnTXTQuanOptionTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_PositionEntry
    void OnCBPositionEntryClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TXT_PositionEntry
    void OnTXTPositionEntryTextUpdated( wxCommandEvent& event );

    /// wxEVT_SET_FOCUS event handler for ID_TXT_PositionEntry
    void OnSetFocus_PositionEntry( wxFocusEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIO_PositionEntry
    void OnRADIOPositionEntrySelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_PositionExitProfit
    void OnCBPositionExitProfitClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TXT_PositionExitProfit
    void OnTXTPositionExitProfitTextUpdated( wxCommandEvent& event );

    /// wxEVT_SET_FOCUS event handler for ID_TXT_PositionExitProfit
    void OnSetFocus_PositionExitProfit( wxFocusEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIO_PositionExitProfit
    void OnRADIOPositionExitProfitSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_PositionExitStop
    void OnCBPositionExitStopClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TXT_PositionExitStop
    void OnTXTPositionExitStopTextUpdated( wxCommandEvent& event );

    /// wxEVT_SET_FOCUS event handler for ID_TXT_PositionExitStop
    void OnSetFocus_PositionExitStop( wxFocusEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIO_PositionExitTop
    void OnRADIOPositionExitTopSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BtnBuy
    void OnBtnBuyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BtnSell
    void OnBtnSellClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BtnCancel
    void OnBtnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BtnClose
    void OnBtnCloseClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for id_radioBase
    void OnIdRadioBaseSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for id_radioSynthLong
    void OnIdRadioSynthLongSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for id_radioCallItm
    void OnIdRadioCallItmSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for id_radioPutOtm
    void OnIdRadioPutOtmSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for id_radioSynthShort
    void OnIdRadioSynthShortSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for id_radioPutItm
    void OnIdRadioPutItmSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBUTTON_SELECTED event handler for id_radioCallOtm
    void OnIdRadioCallOtmSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_Stoch1
    void OnCBStoch1Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_Stoch2
    void OnCBStoch2Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CB_Stoch3
    void OnCBStoch3Click( wxCommandEvent& event );

  void OnKillFocus( wxFocusEvent& event );

  void ClearInstrumentRadioButtons();

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PanelOrderButtons, 1)
