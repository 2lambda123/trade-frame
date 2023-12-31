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
 * File:    PanelOrderButtons.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading
 * Created: February 11, 2022 13:20
 */

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/radiobut.h>
#include <wx/stattext.h>

#include "PanelOrderButtons.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelOrderButtons::PanelOrderButtons() {
  Init();
}

PanelOrderButtons::PanelOrderButtons(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
) {
  Init();
  Create(parent, id, pos, size, style);
}

bool PanelOrderButtons::Create(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
){

    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer()) {
        GetSizer()->SetSizeHints(this);
    }
    //Centre();
    return true;
}

PanelOrderButtons::~PanelOrderButtons() {
}

void PanelOrderButtons::Init() {
  m_eFocus = EFocus::None;
}

void PanelOrderButtons::CreateControls() {

    wxBoxSizer* sizerMain = nullptr;
    wxBoxSizer* sizerPosition = nullptr;
    wxBoxSizer* sizerQuantity = nullptr;
    wxBoxSizer* sizerQuanStock = nullptr;
    wxBoxSizer* sizerQuanFuture = nullptr;
    wxBoxSizer* sizerQuanOption = nullptr;
    wxBoxSizer* sizerPositionEntry = nullptr;
    wxBoxSizer* sizerPositionExitProfit = nullptr;
    wxBoxSizer* sizerPositionExitStop = nullptr;
    wxBoxSizer* sizerBtnOrderTypes = nullptr;
    wxBoxSizer* sizerMarketData = nullptr;
    wxBoxSizer* sizerMDBase = nullptr;
    wxBoxSizer* sizerMDCall1 = nullptr;
    wxBoxSizer* sizerMDPut1 = nullptr;
    wxBoxSizer* sizerMDCall2 = nullptr;
    wxBoxSizer* sizerMDPut2 = nullptr;
    wxBoxSizer* sizerStochastic = nullptr;
    wxBoxSizer* sizerCombos = nullptr;
    wxBoxSizer* sizerBase = nullptr;
    wxBoxSizer* sizerSynthLong = nullptr;
    wxBoxSizer* sizerSynthShort = nullptr;

    wxStaticText* labelQuantity;

    PanelOrderButtons* itemPanel1 = this;

    sizerMain = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(sizerMain);

    sizerPosition = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerPosition, 0, wxALIGN_LEFT|wxLEFT, 6);

    wxStaticText* itemStaticText3 = new wxStaticText( itemPanel1, wxID_STATIC, _("Symbol:"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerPosition->Add(itemStaticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtSymbol = new wxTextCtrl( itemPanel1, ID_TXT_Symbol, wxEmptyString, wxDefaultPosition, wxSize(110, -1), wxTE_READONLY );
    sizerPosition->Add(m_txtSymbol, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxStaticText* itemStaticText5 = new wxStaticText( itemPanel1, wxID_STATIC, _("Quantity:"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerPosition->Add(itemStaticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtQuantity = new wxTextCtrl( itemPanel1, ID_TXT_Quantity, wxEmptyString, wxDefaultPosition, wxSize(65, -1), wxTE_READONLY|wxTE_RIGHT );
    sizerPosition->Add(m_txtQuantity, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxStaticText* itemStaticText7 = new wxStaticText( itemPanel1, wxID_STATIC, _("P/L:"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerPosition->Add(itemStaticText7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_txtProfitLoss = new wxTextCtrl( itemPanel1, ID_TXT_ProfitLoss, wxEmptyString, wxDefaultPosition, wxSize(70, -1), wxTE_READONLY|wxTE_RIGHT );
    sizerPosition->Add(m_txtProfitLoss, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    sizerQuantity = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerQuantity, 0, wxALIGN_LEFT|wxLEFT, 5);

    labelQuantity = new wxStaticText( itemPanel1, wxID_STATIC, _("Order Sz:"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerQuantity->Add(labelQuantity, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    sizerQuanStock = new wxBoxSizer(wxHORIZONTAL);
    sizerQuantity->Add(sizerQuanStock, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_txtQuanStock = new wxTextCtrl( itemPanel1, ID_TXT_QuanStock, _("100"), wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT );
    sizerQuanStock->Add(m_txtQuanStock, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel1, wxID_STATIC, _("Stock"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerQuanStock->Add(itemStaticText13, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxStaticLine* itemStaticLine14 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL|wxNO_BORDER );
    sizerQuantity->Add(itemStaticLine14, 0, wxGROW|wxLEFT|wxRIGHT, 2);

    sizerQuanFuture = new wxBoxSizer(wxHORIZONTAL);
    sizerQuantity->Add(sizerQuanFuture, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_txtQuanFuture = new wxTextCtrl( itemPanel1, ID_TXT_QuanFuture, _("1"), wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT );
    sizerQuanFuture->Add(m_txtQuanFuture, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxStaticText* itemStaticText17 = new wxStaticText( itemPanel1, wxID_STATIC, _("Future"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerQuanFuture->Add(itemStaticText17, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxStaticLine* itemStaticLine18 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL|wxNO_BORDER );
    sizerQuantity->Add(itemStaticLine18, 0, wxGROW|wxLEFT|wxRIGHT, 2);

    sizerQuanOption = new wxBoxSizer(wxHORIZONTAL);
    sizerQuantity->Add(sizerQuanOption, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_txtQuanOption = new wxTextCtrl( itemPanel1, ID_TXT_QuanOption, _("1"), wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT );
    sizerQuanOption->Add(m_txtQuanOption, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxStaticText* itemStaticText21 = new wxStaticText( itemPanel1, wxID_STATIC, _("Option"), wxDefaultPosition, wxDefaultSize, 0 );
    sizerQuanOption->Add(itemStaticText21, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    sizerPositionEntry = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerPositionEntry, 1, wxGROW, 2);

    m_cbEnablePositionEntry = new wxCheckBox( itemPanel1, ID_CB_PositionEntry, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnablePositionEntry->SetValue(true);
    m_cbEnablePositionEntry->Enable(false);
    sizerPositionEntry->Add(m_cbEnablePositionEntry, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    m_txtPricePositionEntry = new wxTextCtrl( itemPanel1, ID_TXT_PositionEntry, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    sizerPositionEntry->Add(m_txtPricePositionEntry, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    wxArrayString m_radioPositionEntryStrings;
    m_radioPositionEntryStrings.Add(_("&Mkt"));
    m_radioPositionEntryStrings.Add(_("&Lmt"));
    m_radioPositionEntryStrings.Add(_("&Lmt TO"));
    m_radioPositionEntryStrings.Add(_("&Stoch"));
    m_radioPositionEntry = new wxRadioBox( itemPanel1, ID_RADIO_PositionEntry, _("Position Entry"), wxDefaultPosition, wxDefaultSize, m_radioPositionEntryStrings, 1, wxRA_SPECIFY_ROWS );
    m_radioPositionEntry->SetSelection(0);
    if (PanelOrderButtons::ShowToolTips())
        m_radioPositionEntry->SetToolTip(_("Market\nLimit\nLimit with Timeout\nStochastic"));
    sizerPositionEntry->Add(m_radioPositionEntry, 1, wxGROW|wxLEFT, 2);

    sizerPositionExitProfit = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerPositionExitProfit, 1, wxGROW, 2);

    m_cbEnableProfitExit = new wxCheckBox( itemPanel1, ID_CB_PositionExitProfit, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnableProfitExit->SetValue(false);
    sizerPositionExitProfit->Add(m_cbEnableProfitExit, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    m_txtPriceProfitExit = new wxTextCtrl( itemPanel1, ID_TXT_PositionExitProfit, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    sizerPositionExitProfit->Add(m_txtPriceProfitExit, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    wxArrayString m_radioExitProfitStrings;
    m_radioExitProfitStrings.Add(_("&Rel"));
    m_radioExitProfitStrings.Add(_("&Abs"));
    m_radioExitProfitStrings.Add(_("St&och"));
    m_radioExitProfit = new wxRadioBox( itemPanel1, ID_RADIO_PositionExitProfit, _("Profit Exit"), wxDefaultPosition, wxDefaultSize, m_radioExitProfitStrings, 1, wxRA_SPECIFY_ROWS );
    m_radioExitProfit->SetSelection(0);
    if (PanelOrderButtons::ShowToolTips())
        m_radioExitProfit->SetToolTip(_("Relative\nAbsolute\nStochastic"));
    sizerPositionExitProfit->Add(m_radioExitProfit, 1, wxGROW|wxLEFT, 2);

    sizerPositionExitStop = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerPositionExitStop, 1, wxGROW|wxBOTTOM, 2);

    m_cbEnableStopExit = new wxCheckBox( itemPanel1, ID_CB_PositionExitStop, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnableStopExit->SetValue(false);
    sizerPositionExitStop->Add(m_cbEnableStopExit, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    m_txtPriceStopExit = new wxTextCtrl( itemPanel1, ID_TXT_PositionExitStop, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    sizerPositionExitStop->Add(m_txtPriceStopExit, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    wxArrayString m_radioExitStopStrings;
    m_radioExitStopStrings.Add(_("Tr&l"));
    m_radioExitStopStrings.Add(_("Trl &Pct"));
    m_radioExitStopStrings.Add(_("St&op"));
    m_radioExitStop = new wxRadioBox( itemPanel1, ID_RADIO_PositionExitTop, _("Stop Exit"), wxDefaultPosition, wxDefaultSize, m_radioExitStopStrings, 1, wxRA_SPECIFY_ROWS );
    m_radioExitStop->SetSelection(0);
    if (PanelOrderButtons::ShowToolTips())
        m_radioExitStop->SetToolTip(_("Trailing Absolute\nTrailing Percent\nStop"));
    sizerPositionExitStop->Add(m_radioExitStop, 1, wxGROW|wxLEFT, 2);

    wxStaticLine* itemStaticLine34 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    sizerMain->Add(itemStaticLine34, 0, wxGROW|wxTOP|wxBOTTOM, 2);

    sizerBtnOrderTypes = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerBtnOrderTypes, 0, wxALIGN_CENTER_HORIZONTAL, 2);

    m_btnBuy = new wxButton( itemPanel1, ID_BtnBuy, _("&Buy"), wxDefaultPosition, wxSize(60, -1), 0 );
    sizerBtnOrderTypes->Add(m_btnBuy, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

    m_btnSell = new wxButton( itemPanel1, ID_BtnSell, _("&Sell"), wxDefaultPosition, wxSize(60, -1), 0 );
    sizerBtnOrderTypes->Add(m_btnSell, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

    m_btnCancel = new wxButton( itemPanel1, ID_BtnCancel, _("C&ancel"), wxDefaultPosition, wxSize(60, -1), 0 );
    m_btnCancel->Enable(false);
    sizerBtnOrderTypes->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

    m_btnClose = new wxButton( itemPanel1, ID_BtnClose, _("&Close"), wxDefaultPosition, wxSize(60, -1), 0 );
    m_btnClose->Enable(false);
    sizerBtnOrderTypes->Add(m_btnClose, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 4);

    wxStaticLine* itemStaticLine40 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    sizerMain->Add(itemStaticLine40, 0, wxGROW|wxTOP|wxBOTTOM, 1);

    sizerCombos = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerCombos, 0, wxALIGN_CENTER_HORIZONTAL, 1);

    sizerBase = new wxBoxSizer(wxVERTICAL);
    sizerCombos->Add(sizerBase, 0, wxALIGN_TOP|wxLEFT|wxRIGHT|wxTOP, 1);

    sizerMDBase = new wxBoxSizer(wxVERTICAL);
    sizerBase->Add(sizerMDBase, 1, wxGROW, 2);

    wxStaticText* itemStaticText1 = new wxStaticText( itemPanel1, wxID_STATIC, _(" "), wxDefaultPosition, wxDefaultSize, 0 );
    sizerMDBase->Add(itemStaticText1, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_radioBase = new wxRadioButton( itemPanel1, id_radioBase, _("base"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_radioBase->SetValue(true);
    sizerMDBase->Add(m_radioBase, 0, wxALIGN_LEFT|wxALL, 2);

    m_txtBaseAsk = new wxStaticText( itemPanel1, ID_TXT_BaseAsk, _("ask"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    sizerMDBase->Add(m_txtBaseAsk, 0, wxGROW|wxRIGHT, 2);

    m_txtBaseBid = new wxStaticText( itemPanel1, ID_TXT_BaseBid, _("bid"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    sizerMDBase->Add(m_txtBaseBid, 0, wxGROW|wxRIGHT, 2);

    wxStaticLine* itemStaticLine47 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    sizerCombos->Add(itemStaticLine47, 0, wxGROW|wxLEFT|wxRIGHT, 1);

    sizerSynthLong = new wxBoxSizer(wxVERTICAL);
    sizerCombos->Add(sizerSynthLong, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxBoxSizer* itemBoxSizer49 = new wxBoxSizer(wxHORIZONTAL);
    sizerSynthLong->Add(itemBoxSizer49, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    m_radioSynthLong = new wxRadioButton( itemPanel1, id_radioSynthLong, _("synth long"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioSynthLong->SetValue(false);
    itemBoxSizer49->Add(m_radioSynthLong, 0, wxALIGN_CENTER_VERTICAL, 1);

    wxBoxSizer* itemBoxSizer51 = new wxBoxSizer(wxHORIZONTAL);
    sizerSynthLong->Add(itemBoxSizer51, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    sizerMDCall1 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer51->Add(sizerMDCall1, 1, wxALIGN_CENTER_VERTICAL, 2);

    m_radioCallItm = new wxRadioButton( itemPanel1, id_radioCallItm, _("itm call"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioCallItm->SetValue(false);
    sizerMDCall1->Add(m_radioCallItm, 0, wxALIGN_LEFT|wxALL, 2);

    m_txtCallItmAsk = new wxStaticText( itemPanel1, ID_TXT_Call1Ask, _("ask"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    sizerMDCall1->Add(m_txtCallItmAsk, 0, wxGROW|wxRIGHT, 2);

    m_txtCallItmBid = new wxStaticText( itemPanel1, ID_TXT_Call1Bid, _("bid"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    sizerMDCall1->Add(m_txtCallItmBid, 0, wxGROW|wxRIGHT, 2);

    sizerMDPut1 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer51->Add(sizerMDPut1, 1, wxALIGN_CENTER_VERTICAL, 2);

    m_radioPutOtm = new wxRadioButton( itemPanel1, id_radioPutOtm, _("otm put"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioPutOtm->SetValue(false);
    sizerMDPut1->Add(m_radioPutOtm, 0, wxALIGN_LEFT|wxALL, 2);

    m_txtPutOtmAsk = new wxStaticText( itemPanel1, ID_TXT_Put1Ask, _("ask"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    sizerMDPut1->Add(m_txtPutOtmAsk, 0, wxGROW|wxRIGHT, 2);

    m_txtPutOtmBid = new wxStaticText( itemPanel1, ID_TXT_Put1Bid, _("bid"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    sizerMDPut1->Add(m_txtPutOtmBid, 0, wxGROW|wxRIGHT, 2);

    wxStaticLine* itemStaticLine60 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    sizerCombos->Add(itemStaticLine60, 0, wxGROW|wxLEFT|wxRIGHT, 1);

    sizerSynthShort = new wxBoxSizer(wxVERTICAL);
    sizerCombos->Add(sizerSynthShort, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxBoxSizer* itemBoxSizer62 = new wxBoxSizer(wxHORIZONTAL);
    sizerSynthShort->Add(itemBoxSizer62, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    m_radioSynthShort = new wxRadioButton( itemPanel1, id_radioSynthShort, _("synth short"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioSynthShort->SetValue(false);
    itemBoxSizer62->Add(m_radioSynthShort, 0, wxALIGN_CENTER_VERTICAL, 1);

    wxBoxSizer* itemBoxSizer64 = new wxBoxSizer(wxHORIZONTAL);
    sizerSynthShort->Add(itemBoxSizer64, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 1);

    sizerMDPut2 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer64->Add(sizerMDPut2, 1, wxALIGN_CENTER_VERTICAL, 2);

    m_radioPutItm = new wxRadioButton( itemPanel1, id_radioPutItm, _("itm put"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioPutItm->SetValue(false);
    sizerMDPut2->Add(m_radioPutItm, 0, wxALIGN_LEFT|wxALL, 2);

    m_txtPutItmAsk = new wxStaticText( itemPanel1, ID_TXT_Put2Ask, _("ask"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    sizerMDPut2->Add(m_txtPutItmAsk, 0, wxGROW|wxRIGHT, 2);

    m_txtPutItmBid = new wxStaticText( itemPanel1, ID_TXT_Put2Bid, _("bid"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    sizerMDPut2->Add(m_txtPutItmBid, 0, wxGROW|wxRIGHT, 2);

    sizerMDCall2 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer64->Add(sizerMDCall2, 1, wxALIGN_CENTER_VERTICAL, 2);

    m_radioCallOtm = new wxRadioButton( itemPanel1, id_radioCallOtm, _("otm call"), wxDefaultPosition, wxDefaultSize, 0 );
    m_radioCallOtm->SetValue(false);
    sizerMDCall2->Add(m_radioCallOtm, 0, wxALIGN_LEFT|wxALL, 2);

    m_txtCallOtmAsk = new wxStaticText( itemPanel1, ID_TXT_Call2Ask, _("ask"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    sizerMDCall2->Add(m_txtCallOtmAsk, 0, wxGROW|wxRIGHT, 2);

    m_txtCallOtmBid = new wxStaticText( itemPanel1, ID_TXT_Call2Bid, _("bid"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
    sizerMDCall2->Add(m_txtCallOtmBid, 0, wxGROW|wxRIGHT, 2);

    wxStaticLine* itemStaticLine73 = new wxStaticLine( itemPanel1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    sizerMain->Add(itemStaticLine73, 0, wxGROW|wxTOP|wxBOTTOM, 1);

    sizerStochastic = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(sizerStochastic, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxBOTTOM, 2);

    m_cbEnableStoch1 = new wxCheckBox( itemPanel1, ID_CB_Stoch1, _("Stoch 1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnableStoch1->SetValue(false);
    sizerStochastic->Add(m_cbEnableStoch1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_cbEnableStoch2 = new wxCheckBox( itemPanel1, ID_CB_Stoch2, _("Stoch 2"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnableStoch2->SetValue(false);
    sizerStochastic->Add(m_cbEnableStoch2, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_cbEnableStoch3 = new wxCheckBox( itemPanel1, ID_CB_Stoch3, _("Stoch 3"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbEnableStoch3->SetValue(false);
    sizerStochastic->Add(m_cbEnableStoch3, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

  m_txtPricePositionEntry->Connect(ID_TXT_PositionEntry, wxEVT_SET_FOCUS, wxFocusEventHandler(PanelOrderButtons::OnSetFocus_PositionEntry), NULL, this);
  m_txtPricePositionEntry->Connect(ID_TXT_PositionEntry, wxEVT_KILL_FOCUS, wxFocusEventHandler(PanelOrderButtons::OnKillFocus), NULL, this);

  m_txtPriceProfitExit->Connect(ID_TXT_PositionExitProfit, wxEVT_SET_FOCUS, wxFocusEventHandler(PanelOrderButtons::OnSetFocus_PositionExitProfit), NULL, this);
  m_txtPriceProfitExit->Connect(ID_TXT_PositionExitProfit, wxEVT_KILL_FOCUS, wxFocusEventHandler(PanelOrderButtons::OnKillFocus), NULL, this);

  m_txtPriceStopExit->Connect(ID_TXT_PositionExitStop, wxEVT_SET_FOCUS, wxFocusEventHandler(PanelOrderButtons::OnSetFocus_PositionExitStop), NULL, this);
  m_txtPriceStopExit->Connect(ID_TXT_PositionExitStop, wxEVT_KILL_FOCUS, wxFocusEventHandler(PanelOrderButtons::OnKillFocus), NULL, this);

  Bind( wxEVT_DESTROY, &PanelOrderButtons::OnDestroy, this );

  Bind( wxEVT_SET_FOCUS, &PanelOrderButtons::OnFocusChange, this );
  Bind( wxEVT_KILL_FOCUS, &PanelOrderButtons::OnFocusChange, this );

  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnBuyClick, this, ID_BtnBuy );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnSellClick, this, ID_BtnSell );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnCloseClick, this, ID_BtnClose );
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &PanelOrderButtons::OnBtnCancelClick, this, ID_BtnCancel );

  Bind( wxEVT_RADIOBOX, &PanelOrderButtons::OnRADIOPositionEntrySelected, this, ID_RADIO_PositionEntry );
  Bind( wxEVT_RADIOBOX, &PanelOrderButtons::OnRADIOPositionExitProfitSelected, this, ID_RADIO_PositionExitProfit );
  Bind( wxEVT_RADIOBOX, &PanelOrderButtons::OnRADIOPositionExitTopSelected, this, ID_RADIO_PositionExitTop );

  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBPositionEntryClick, this, ID_CB_PositionEntry );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBPositionExitProfitClick, this, ID_CB_PositionExitProfit );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBPositionExitStopClick, this, ID_CB_PositionExitStop );

  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelOrderButtons::OnIdRadioBaseSelected, this, id_radioBase );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelOrderButtons::OnIdRadioSynthLongSelected, this, id_radioSynthLong );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelOrderButtons::OnIdRadioCallItmSelected, this, id_radioCallItm );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelOrderButtons::OnIdRadioPutOtmSelected, this, id_radioPutOtm );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelOrderButtons::OnIdRadioSynthShortSelected, this, id_radioSynthShort );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelOrderButtons::OnIdRadioPutItmSelected, this, id_radioPutItm );
  Bind( wxEVT_COMMAND_RADIOBUTTON_SELECTED, &PanelOrderButtons::OnIdRadioCallOtmSelected, this, id_radioCallOtm );

  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBStoch1Click, this, ID_CB_Stoch1 );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBStoch2Click, this, ID_CB_Stoch2 );
  Bind( wxEVT_COMMAND_CHECKBOX_CLICKED, &PanelOrderButtons::OnCBStoch3Click, this, ID_CB_Stoch3 );

  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTPositionEntryTextUpdated, this, ID_TXT_PositionEntry );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTPositionExitProfitTextUpdated, this, ID_TXT_PositionExitProfit );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTPositionExitStopTextUpdated, this, ID_TXT_PositionExitStop );

  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTQuanStockTextUpdated, this, ID_TXT_QuanStock );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTQuanFutureTextUpdated, this, ID_TXT_QuanFuture );
  Bind( wxEVT_COMMAND_TEXT_UPDATED, &PanelOrderButtons::OnTXTQuanOptionTextUpdated, this, ID_TXT_QuanOption );

}

void PanelOrderButtons::Set(
  fBtnOrder_t&& fBtnOrderBuy, // Buy
  fBtnOrder_t&& fBtnOrderSell, // Sell
  fBtnOrder_t&& fBtnOrderClose, // Sell
  fBtnOrder_t&& fBtnOrderCancel  // CancelAll
) {
  m_fBtnOrderBuy = std::move( fBtnOrderBuy );
  m_fBtnOrderSell = std::move( fBtnOrderSell );
  m_fBtnOrderClose = std::move( fBtnOrderClose );
  m_fBtnOrderCancel = std::move( fBtnOrderCancel );
}

void PanelOrderButtons::Trigger( PanelOrderButtons_Order::EOrderMethod method ) {
  switch ( method ) {
    case PanelOrderButtons_Order::EOrderMethod::Buy:
      ClickBuy();
      break;
    case PanelOrderButtons_Order::EOrderMethod::Sell:
      ClickSell();
      break;
    case PanelOrderButtons_Order::EOrderMethod::Cancel:
      assert( false );
      break;
  }
}

void PanelOrderButtons::SetPriceAtFocus( const std::string& sText ) {
  switch ( m_eFocus ) {
    case EFocus::Price:
      m_txtPricePositionEntry->SetValue( sText );
      m_radioPositionEntry->SetSelection( 1 );
      m_order.m_ePositionEntryMethod = PanelOrderButtons_Order::EPositionEntryMethod::LimitOnly;
      break;
    case EFocus::Profit:
      m_txtPriceProfitExit->SetValue( sText );
      m_radioExitProfit->SetSelection( 1 );
      m_cbEnableProfitExit->SetValue( true );
      m_order.m_ePositionExitProfitMethod = PanelOrderButtons_Order::EPositionExitProfitMethod::Absolute;
      break;
    case EFocus::Stop:
      m_txtPriceStopExit->SetValue( sText );
      m_radioExitStop->SetSelection( 2 );
      m_cbEnableStopExit->SetValue( true );
      m_order.m_ePositionExitStopMethod = PanelOrderButtons_Order::EPositionExitStopMethod::Stop;
      break;
    case EFocus::None:
      break;
  }
}

void PanelOrderButtons::Update( const PanelOrderButtons_MarketData& data ) {

  if ( data.m_bOptionPresent ) {
    CallAfter(
      [this,
        //sBase = std::move( data.m_sBase ),
        sBaseAsk = std::move(data.m_sBaseAsk )
      , sBaseBid = std::move( data.m_sBaseBid )

      , sCallItm    = std::move( data.m_sCall1 )
      , sCallItmAsk = std::move( data.m_sCall1Ask )
      , sCallItmBid = std::move( data.m_sCall1Bid )

      , sPutOtm    = std::move( data.m_sPut1 )
      , sPutOtmAsk = std::move( data.m_sPut1Ask )
      , sPutOtmBid = std::move( data.m_sPut1Bid )

      , sPutItm    = std::move( data.m_sPut2 )
      , sPutItmAsk = std::move( data.m_sPut2Ask )
      , sPutItmBid = std::move( data.m_sPut2Bid )

      , sCallOtm    = std::move( data.m_sCall2 )
      , sCallOtmAsk = std::move( data.m_sCall2Ask )
      , sCallOtmBid = std::move( data.m_sCall2Bid )
      ](){
        //m_txtBase->SetLabel( sBase );
        m_txtBaseAsk->SetLabel( sBaseAsk );
        m_txtBaseBid->SetLabel( sBaseBid );

        m_radioCallItm->SetLabel( sCallItm );
        m_txtCallItmAsk->SetLabel( sCallItmAsk );
        m_txtCallItmBid->SetLabel( sCallItmBid );

        m_radioPutOtm->SetLabel( sPutOtm );
        m_txtPutOtmAsk->SetLabel( sPutOtmAsk );
        m_txtPutOtmBid->SetLabel( sPutOtmBid);

        m_radioPutItm->SetLabel( sPutItm );
        m_txtPutItmAsk->SetLabel( sPutItmAsk );
        m_txtPutItmBid->SetLabel( sPutItmBid );

        m_radioCallOtm->SetLabel( sCallOtm );
        m_txtCallOtmAsk->SetLabel( sCallOtmAsk );
        m_txtCallOtmBid->SetLabel( sCallOtmBid );

      } );
  }
  else {
    CallAfter(
      [this,
        //sBase = std::move( data.m_sBase ),
        sBaseAsk = std::move(data.m_sBaseAsk )
      , sBaseBid = std::move( data.m_sBaseBid )
      ](){
        //m_txtBase->SetLabel( sBase );
        m_txtBaseAsk->SetLabel( sBaseAsk );
        m_txtBaseBid->SetLabel( sBaseBid );
      } );
  }



}

void PanelOrderButtons::Update( const PanelOrderButtons_PositionData& data ) {
  CallAfter(
    [this,
         sSymbol = std::move( data.m_sSymbol ),
         sQuantity = std::move(data.m_sQuantity ),
         sProfitLoss = std::move( data.m_sProfitLoss )
         ](){
      m_txtSymbol->SetValue( std::move( sSymbol ) );
      m_txtQuantity->SetValue( std::move( sQuantity ) );
      m_txtProfitLoss->SetValue( std::move( sProfitLoss ) );
    } );
}

bool PanelOrderButtons::ValidateFields() {
  bool bOk( true );
  if ( m_order.m_bPositionEntryEnable ) {
    if ( PanelOrderButtons_Order::EPositionEntryMethod::LimitOnly == m_order.m_ePositionEntryMethod ) {
      if ( 0 == m_order.m_sPositionEntryValue.size() ) {
        std::cout << "position entry: requires value" << std::endl;
        bOk = false;
      }
    }
  }
  if ( m_order.m_bPositionEntryEnable ) {
    if ( PanelOrderButtons_Order::EPositionEntryMethod::LimitTimeOut == m_order.m_ePositionEntryMethod ) {
      if ( 0 == m_order.m_sPositionEntryValue.size() ) {
        std::cout << "position entry: requires value" << std::endl;
        bOk = false;
      }
    }
  }
  if ( m_order.m_bPositionExitProfitEnable ) {
    if ( PanelOrderButtons_Order::EPositionExitProfitMethod::Absolute == m_order.m_ePositionExitProfitMethod ) {
      if ( 0 == m_order.m_sPositionExitProfitValue.size() ) {
        std::cout << "position exit profit (abs): requires value" << std::endl;
        bOk = false;
      }
    }
    if ( PanelOrderButtons_Order::EPositionExitProfitMethod::Relative == m_order.m_ePositionExitProfitMethod ) {
      if ( 0 == m_order.m_sPositionExitProfitValue.size() ) {
        std::cout << "position exit profit (rel): requires value" << std::endl;
        bOk = false;
      }
    }
  }
  if ( m_order.m_bPositionExitStopEnable ) {
    if ( 0 == m_order.m_sPositionExitStopValue.size() ) {
      std::cout << "position exit stop: requires value" << std::endl;
      bOk = false;
    }
  }
  if ( PanelOrderButtons_Order::EPositionEntryMethod::Stoch == m_order.m_ePositionEntryMethod ) {
    if ( m_order.m_bStochastic1 | m_order.m_bStochastic2 | m_order.m_bStochastic3 ) {}
    else {
      std::cout << "position entry (stochastic): requires at least one" << std::endl;
      bOk = false;
    }
  }
  if ( PanelOrderButtons_Order::EPositionExitProfitMethod::Stoch == m_order.m_ePositionExitProfitMethod ) {
    if ( m_order.m_bStochastic1 | m_order.m_bStochastic2 | m_order.m_bStochastic3 ) {}
    else {
      std::cout << "position exit profit (stochastic): requires at least one" << std::endl;
      bOk = false;
    }
  }
  return bOk;
}

void PanelOrderButtons::ClickBuy() {
  if ( ValidateFields() ) {
    if ( m_fBtnOrderBuy ) {
      wxColour colour = m_btnBuy->GetForegroundColour();
      m_btnBuy->SetForegroundColour( *wxGREEN );
      m_fBtnOrderBuy(
        m_order,
        [this,colour](){ // fBtnDone_t
          m_btnBuy->SetForegroundColour( colour );
        } );
    }
  }
}

void PanelOrderButtons::OnBtnBuyClick( wxCommandEvent& event ) {
  ClickBuy();
  event.Skip();
}

void PanelOrderButtons::ClickSell() {
  if ( ValidateFields() ) {
    if ( m_fBtnOrderSell ) {
      wxColour colour = m_btnSell->GetForegroundColour();
      m_btnSell->SetForegroundColour( *wxGREEN );
      m_fBtnOrderSell(
        m_order,
        [this,colour](){ // fBtnDone_t
          m_btnSell->SetForegroundColour( colour );
        } );
    }
  }
}

void PanelOrderButtons::OnBtnSellClick( wxCommandEvent& event ) {
  ClickSell();
  event.Skip();
}

void PanelOrderButtons::OnBtnCloseClick( wxCommandEvent& event ) {
  //if ( ValidateFields() ) { // need this?
    if ( m_fBtnOrderClose ) {
      wxColour colour = m_btnClose->GetForegroundColour();
      m_btnClose->SetForegroundColour( *wxGREEN );
      m_fBtnOrderClose(
        m_order,
        [this,colour](){ // fBtnDone_t
          m_btnClose->SetForegroundColour( colour );
        } );
    }
  //}
  event.Skip();
}

void PanelOrderButtons::OnBtnCancelClick( wxCommandEvent& event ) {
  if ( m_fBtnOrderCancel ) {
    wxColour colour = m_btnCancel->GetForegroundColour();
    m_btnCancel->SetForegroundColour( *wxGREEN );
    m_fBtnOrderCancel(
      m_order,
      [this,colour](){ // fBtnDone_t
        m_btnCancel->SetForegroundColour( colour );
      } );
  }
  event.Skip();
}

void PanelOrderButtons::OnFocusChange( wxFocusEvent& event ) {
  event.Skip();
}

void PanelOrderButtons::OnDestroy( wxWindowDestroyEvent& event ) {
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  event.Skip();  // auto followed by Destroy();
}

// == Position Entry

void PanelOrderButtons::OnCBPositionEntryClick( wxCommandEvent& event ) {
  m_order.m_bPositionEntryEnable = m_cbEnablePositionEntry->IsChecked();
  event.Skip();
}

void PanelOrderButtons::OnTXTPositionEntryTextUpdated( wxCommandEvent& event ) {
  m_eFocus = EFocus::None;
  m_order.m_sPositionEntryValue = m_txtPricePositionEntry->GetValue();
  event.Skip();
}

void PanelOrderButtons::OnRADIOPositionEntrySelected( wxCommandEvent& event ) {
  //std::string s( event.GetString() );
  //std::cout << "order: " << s << "," << event.GetSelection() << std::endl;
  switch( m_radioPositionEntry->GetSelection() ) {
    case 0:
      m_order.m_ePositionEntryMethod = PanelOrderButtons_Order::EPositionEntryMethod::Market;
      break;
    case 1:
      m_order.m_ePositionEntryMethod = PanelOrderButtons_Order::EPositionEntryMethod::LimitOnly;
      break;
    case 2:
      m_order.m_ePositionEntryMethod = PanelOrderButtons_Order::EPositionEntryMethod::LimitTimeOut;
      break;
    case 3:
      m_order.m_ePositionEntryMethod = PanelOrderButtons_Order::EPositionEntryMethod::Stoch;
      break;
  }
  event.Skip();
}

// == Position Exit for Profit

void PanelOrderButtons::OnCBPositionExitProfitClick( wxCommandEvent& event ) {
  event.Skip();
  m_order.m_bPositionExitProfitEnable = m_cbEnableProfitExit->IsChecked();
  event.Skip();
}

void PanelOrderButtons::OnTXTPositionExitProfitTextUpdated( wxCommandEvent& event ) {
  m_eFocus = EFocus::None;
  m_order.m_sPositionExitProfitValue = m_txtPriceProfitExit->GetValue();
  event.Skip();
}

void PanelOrderButtons::OnRADIOPositionExitProfitSelected( wxCommandEvent& event ) {
  switch ( m_radioExitProfit->GetSelection() ) {
    case 0:
      m_order.m_ePositionExitProfitMethod = PanelOrderButtons_Order::EPositionExitProfitMethod::Relative;
      break;
    case 1:
      m_order.m_ePositionExitProfitMethod = PanelOrderButtons_Order::EPositionExitProfitMethod::Absolute;
      break;
    case 2:
      m_order.m_ePositionExitProfitMethod = PanelOrderButtons_Order::EPositionExitProfitMethod::Stoch;
  }
  event.Skip();
}

// == Position Exit with Stop

void PanelOrderButtons::OnCBPositionExitStopClick( wxCommandEvent& event ) {
  m_order.m_bPositionExitStopEnable = m_cbEnableStopExit->IsChecked();
  event.Skip();
}

void PanelOrderButtons::OnTXTPositionExitStopTextUpdated( wxCommandEvent& event ) {
  m_eFocus = EFocus::None;
  m_order.m_sPositionExitStopValue = m_txtPriceStopExit->GetValue();
  event.Skip();
}

void PanelOrderButtons::OnRADIOPositionExitTopSelected( wxCommandEvent& event ) {
  switch( m_radioExitStop->GetSelection() ) {
    case 0:
      m_order.m_ePositionExitStopMethod = PanelOrderButtons_Order::EPositionExitStopMethod::TrailingAbsolute;
      break;
    case 1:
      m_order.m_ePositionExitStopMethod = PanelOrderButtons_Order::EPositionExitStopMethod::TrailingPercent;
      break;
    case 2:
      m_order.m_ePositionExitStopMethod = PanelOrderButtons_Order::EPositionExitStopMethod::Stop;
      break;
  }
  event.Skip();
}

// == Instrument

void PanelOrderButtons::OnIdRadioBaseSelected( wxCommandEvent& event ) {
  ClearInstrumentRadioButtons();
  m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::Underlying;
  m_radioBase->SetValue( true );
}

void PanelOrderButtons::OnIdRadioSynthLongSelected( wxCommandEvent& event ) {
  ClearInstrumentRadioButtons();
  m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::SynthLong;
  m_radioSynthLong->SetValue( true );
}

void PanelOrderButtons::OnIdRadioCallItmSelected( wxCommandEvent& event ) {
  ClearInstrumentRadioButtons();
  m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::CallItm;
  m_radioCallItm->SetValue( true );
}

void PanelOrderButtons::OnIdRadioPutOtmSelected( wxCommandEvent& event ) {
  ClearInstrumentRadioButtons();
  m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::PutOtm;
  m_radioPutOtm->SetValue( true );
}

void PanelOrderButtons::OnIdRadioSynthShortSelected( wxCommandEvent& event ) {
  ClearInstrumentRadioButtons();
  m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::SynthShort;
  m_radioSynthShort->SetValue( true );
}

void PanelOrderButtons::OnIdRadioPutItmSelected( wxCommandEvent& event ) {
  ClearInstrumentRadioButtons();
  m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::PutItm;
  m_radioPutItm->SetValue( true );
}

void PanelOrderButtons::OnIdRadioCallOtmSelected( wxCommandEvent& event ) {
  ClearInstrumentRadioButtons();
  m_order.m_eInstrument = PanelOrderButtons_Order::EInstrument::CallOtm;
  m_radioCallOtm->SetValue( true );
}

void PanelOrderButtons::ClearInstrumentRadioButtons() {
  m_radioBase->SetValue( false );
  m_radioSynthLong->SetValue( false );
  m_radioCallItm->SetValue( false );
  m_radioPutOtm->SetValue( false );
  m_radioSynthShort->SetValue( false );
  m_radioPutItm->SetValue( false );
  m_radioCallOtm->SetValue( false );
}

// == Stochastic selection

void PanelOrderButtons::OnCBStoch1Click( wxCommandEvent& event ) {
  m_order.m_bStochastic1 = m_cbEnableStoch1->IsChecked();
  event.Skip();
}

void PanelOrderButtons::OnCBStoch2Click( wxCommandEvent& event ) {
  m_order.m_bStochastic2 = m_cbEnableStoch2->IsChecked();
  event.Skip();
}

void PanelOrderButtons::OnCBStoch3Click( wxCommandEvent& event ) {
  m_order.m_bStochastic3 = m_cbEnableStoch3->IsChecked();
  event.Skip();
}

// == Quantity Selection

void PanelOrderButtons::OnTXTQuanStockTextUpdated( wxCommandEvent& event ) {
  m_order.m_sQuanStock = m_txtQuanStock->GetValue();
  event.Skip();
}

void PanelOrderButtons::OnTXTQuanFutureTextUpdated( wxCommandEvent& event ) {
  m_order.m_sQuanFuture = m_txtQuanFuture->GetValue();
  event.Skip();
}

void PanelOrderButtons::OnTXTQuanOptionTextUpdated( wxCommandEvent& event ) {
  m_order.m_sQuanOption = m_txtQuanOption->GetValue();
  event.Skip();
}

void PanelOrderButtons::OnSetFocus_PositionEntry( wxFocusEvent& event ) {
  m_eFocus = EFocus::Price;
  event.Skip();
}

void PanelOrderButtons::OnSetFocus_PositionExitProfit( wxFocusEvent& event ) {
  m_eFocus = EFocus::Profit;
  event.Skip();
}

void PanelOrderButtons::OnSetFocus_PositionExitStop( wxFocusEvent& event ) {
  m_eFocus = EFocus::Stop;
  event.Skip();
}

void PanelOrderButtons::OnKillFocus( wxFocusEvent& event ) {
  //m_eFocus = EFocus::None;
  event.Skip();
}

} // namespace tf
} // namespace ou
