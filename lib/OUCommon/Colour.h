/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

namespace ou {

// http://en.wikipedia.org/wiki/Color#cite_note-0

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef DWORD COLORREF;

#ifdef RGB
#undef RGB
#endif

#ifndef WINRGB
// windows COLORREF is backwards from what ChartDir is expecting
// ChartDir:
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#else
// from WinGDI.h:
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif

// we may get into problems if this file is used for windows colours as well as ChartDir colours
// or template it or type traits it or use some sort of define

// may need to create two color namespaces if used in the same include hiearchy

namespace Colour {
  enum EColour {
    IndianRed = RGB(0xCD,0x5C,0x5C),
    LightCoral = RGB(0xF0,0x80,0x80),
    Salmon = RGB(0xFA,0x80,0x72),
    DarkSalmon = RGB(0xE9,0x96,0x7A),
    LightSalmon = RGB(0xFF,0xA0,0x7A),
    Crimson = RGB(0xDC,0x14,0x3C),
    Red = RGB(0xFF,0x00,0x00),
    FireBrick = RGB(0xB2,0x22,0x22),
    DarkRed = RGB(0x8B,0x00,0x00),
    Pink = RGB(0xFF,0xC0,0xCB),
    LightPink = RGB(0xFF,0xB6,0xC1),
    HotPink = RGB(0xFF,0x69,0xB4),
    DeepPink = RGB(0xFF,0x14,0x93),
    MediumVioletRed = RGB(0xC7,0x15,0x85),
    PaleVioletRed = RGB(0xDB,0x70,0x93),
    Coral = RGB(0xFF,0x7F,0x50),
    Tomato = RGB(0xFF,0x63,0x47),
    OrangeRed = RGB(0xFF,0x45,0x00),
    DarkOrange = RGB(0xFF,0x8C,0x00),
    Orange = RGB(0xFF,0xA5,0x00),
    Gold = RGB(0xFF,0xD7,0x00),
    Yellow = RGB(0xFF,0xFF,0x00),
    LightYellow = RGB(0xFF,0xFF,0xE0),
    LemonChiffon = RGB(0xFF,0xFA,0xCD),
    LightGoldenrodYellow = RGB(0xFA,0xFA,0xD2),
    PapayaWhip = RGB(0xFF,0xEF,0xD5),
    Moccasin = RGB(0xFF,0xE4,0xB5),
    PeachPuff = RGB(0xFF,0xDA,0xB9),
    PaleGoldenrod = RGB(0xEE,0xE8,0xAA),
    Khaki = RGB(0xF0,0xE6,0x8C),
    DarkKhaki = RGB(0xBD,0xB7,0x6B),
    Lavender = RGB(0xE6,0xE6,0xFA),
    Thistle = RGB(0xD8,0xBF,0xD8),
    Plum = RGB(0xDD,0xA0,0xDD),
    Violet = RGB(0xEE,0x82,0xEE),
    Orchid = RGB(0xDA,0x70,0xD6),
    Fuchsia = RGB(0xFF,0x00,0xFF),
    Magenta = RGB(0xFF,0x00,0xFF),
    MediumOrchid = RGB(0xBA,0x55,0xD3),
    MediumPurple = RGB(0x93,0x70,0xDB),
    BlueViolet = RGB(0x8A,0x2B,0xE2),
    DarkViolet = RGB(0x94,0x00,0xD3),
    DarkOrchid = RGB(0x99,0x32,0xCC),
    DarkMagenta = RGB(0x8B,0x00,0x8B),
    Purple = RGB(0x80,0x00,0x80),
    Indigo = RGB(0x4B,0x00,0x82),
    SlateBlue = RGB(0x6A,0x5A,0xCD),
    DarkSlateBlue = RGB(0x48,0x3D,0x8B),
    GreenYellow = RGB(0xAD,0xFF,0x2F),
    Chartreuse = RGB(0x7F,0xFF,0x00),
    LawnGreen = RGB(0x7C,0xFC,0x00),
    Lime = RGB(0x00,0xFF,0x00),
    LimeGreen = RGB(0x32,0xCD,0x32),
    PaleGreen = RGB(0x98,0xFB,0x98),
    LightGreen = RGB(0x90,0xEE,0x90),
    MediumSpringGreen = RGB(0x00,0xFA,0x9A),
    SpringGreen = RGB(0x00,0xFF,0x7F),
    MediumSeaGreen = RGB(0x3C,0xB3,0x71),
    SeaGreen = RGB(0x2E,0x8B,0x57),
    ForestGreen = RGB(0x22,0x8B,0x22),
    Green = RGB(0x00,0x80,0x00),
    DarkGreen = RGB(0x00,0x64,0x00),
    YellowGreen = RGB(0x9A,0xCD,0x32),
    OliveDrab = RGB(0x6B,0x8E,0x23),
    Olive = RGB(0x80,0x80,0x00),
    DarkOliveGreen = RGB(0x55,0x6B,0x2F),
    MediumAquamarine = RGB(0x66,0xCD,0xAA),
    DarkSeaGreen = RGB(0x8F,0xBC,0x8F),
    LightSeaGreen = RGB(0x20,0xB2,0xAA),
    DarkCyan = RGB(0x00,0x8B,0x8B),
    Teal = RGB(0x00,0x80,0x80),
    Aqua = RGB(0x00,0xFF,0xFF),
    Cyan = RGB(0x00,0xFF,0xFF),
    LightCyan = RGB(0xE0,0xFF,0xFF),
    PaleTurquoise = RGB(0xAF,0xEE,0xEE),
    Aquamarine = RGB(0x7F,0xFF,0xD4),
    Turquoise = RGB(0x40,0xE0,0xD0),
    MediumTurquoise = RGB(0x48,0xD1,0xCC),
    DarkTurquoise = RGB(0x00,0xCE,0xD1),
    CadetBlue = RGB(0x5F,0x9E,0xA0),
    SteelBlue = RGB(0x46,0x82,0xB4),
    LightSteelBlue = RGB(0xB0,0xC4,0xDE),
    PowderBlue = RGB(0xB0,0xE0,0xE6),
    LightBlue = RGB(0xAD,0xD8,0xE6),
    SkyBlue = RGB(0x87,0xCE,0xEB),
    LightSkyBlue = RGB(0x87,0xCE,0xFA),
    DeepSkyBlue = RGB(0x00,0xBF,0xFF),
    DodgerBlue = RGB(0x1E,0x90,0xFF),
    CornflowerBlue = RGB(0x64,0x95,0xED),
    MediumSlateBlue = RGB(0x7B,0x68,0xEE),
    RoyalBlue = RGB(0x41,0x69,0xE1),
    Blue = RGB(0x00,0x00,0xFF),
    MediumBlue = RGB(0x00,0x00,0xCD),
    DarkBlue = RGB(0x00,0x00,0x8B),
    Navy = RGB(0x00,0x00,0x80),
    MidnightBlue = RGB(0x19,0x19,0x70),
    Cornsilk = RGB(0xFF,0xF8,0xDC),
    BlanchedAlmond = RGB(0xFF,0xEB,0xCD),
    Bisque = RGB(0xFF,0xE4,0xC4),
    NavajoWhite = RGB(0xFF,0xDE,0xAD),
    Wheat = RGB(0xF5,0xDE,0xB3),
    BurlyWood = RGB(0xDE,0xB8,0x87),
    Tan = RGB(0xD2,0xB4,0x8C),
    RosyBrown = RGB(0xBC,0x8F,0x8F),
    SandyBrown = RGB(0xF4,0xA4,0x60),
    Goldenrod = RGB(0xDA,0xA5,0x20),
    DarkGoldenrod = RGB(0xB8,0x86,0x0B),
    Peru = RGB(0xCD,0x85,0x3F),
    Chocolate = RGB(0xD2,0x69,0x1E),
    SaddleBrown = RGB(0x8B,0x45,0x13),
    Sienna = RGB(0xA0,0x52,0x2D),
    Brown = RGB(0xA5,0x2A,0x2A),
    Maroon = RGB(0x80,0x00,0x00),
    White = RGB(0xFF,0xFF,0xFF),
    Snow = RGB(0xFF,0xFA,0xFA),
    Honeydew = RGB(0xF0,0xFF,0xF0),
    MintCream = RGB(0xF5,0xFF,0xFA),
    Azure = RGB(0xF0,0xFF,0xFF),
    AliceBlue = RGB(0xF0,0xF8,0xFF),
    GhostWhite = RGB(0xF8,0xF8,0xFF),
    WhiteSmoke = RGB(0xF5,0xF5,0xF5),
    Seashell = RGB(0xFF,0xF5,0xEE),
    Beige = RGB(0xF5,0xF5,0xDC),
    OldLace = RGB(0xFD,0xF5,0xE6),
    FloralWhite = RGB(0xFF,0xFA,0xF0),
    Ivory = RGB(0xFF,0xFF,0xF0),
    AntiqueWhite = RGB(0xFA,0xEB,0xD7),
    Linen = RGB(0xFA,0xF0,0xE6),
    LavenderBlush = RGB(0xFF,0xF0,0xF5),
    MistyRose = RGB(0xFF,0xE4,0xE1),
    Gainsboro = RGB(0xDC,0xDC,0xDC),
    LightGrey = RGB(0xD3,0xD3,0xD3),
    Silver = RGB(0xC0,0xC0,0xC0),
    DarkGray = RGB(0xA9,0xA9,0xA9),
    Gray = RGB(0x80,0x80,0x80),
    DimGray = RGB(0x69,0x69,0x69),
    LightSlateGray = RGB(0x77,0x88,0x99),
    SlateGray = RGB(0x70,0x80,0x90),
    DarkSlateGray = RGB(0x2F,0x4F,0x4F),
    Black = RGB(0x00,0x00,0x00)
  };
}

} // ou