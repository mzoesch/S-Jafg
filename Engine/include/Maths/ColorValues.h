// Copyright mzoesch. All rights reserved.

#ifndef PRIVATE_JAFG_COLOR_VALUE
    #error "PRIVATE_JAFG_COLOR_VALUE is not defined."
#endif /* !PRIVATE_JAFG_COLOR_VALUE */

#ifndef PRIVATE_JAFG_COLOR_VALUE_CONV
    #error "PRIVATE_JAFG_COLOR_VALUE_CONV is not defined."
#endif /* !PRIVATE_JAFG_COLOR_VALUE_CONV */

//#
//# The following colors do not follow any CSS standard. And are just part here for
//# missing color values in the standard.
//#
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkerGray          { PRIVATE_JAFG_COLOR_VALUE_CONV(0x1A, 0x1A, 0x1A, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkerGrey       { PRIVATE_JAFG_COLOR_VALUE_CONV(0x1A, 0x1A, 0x1A, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE NotSoDarkGray       { PRIVATE_JAFG_COLOR_VALUE_CONV(0x2B, 0x2B, 0x2B, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE NotSoDarkGrey    { PRIVATE_JAFG_COLOR_VALUE_CONV(0x2B, 0x2B, 0x2B, 0xFF) };

//#
//# The following colors follow the CSS standard.
//# https://developer.mozilla.org/en-US/docs/Web/CSS/named-color
//#
inline constexpr PRIVATE_JAFG_COLOR_VALUE Transparent{ PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x00, 0x00, 0x00) };

//#
//# BEGIN Standard
//#
inline constexpr PRIVATE_JAFG_COLOR_VALUE Black       { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x00, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Silver      { PRIVATE_JAFG_COLOR_VALUE_CONV(0xC0, 0xC0, 0xC0, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Gray        { PRIVATE_JAFG_COLOR_VALUE_CONV(0x80, 0x80, 0x80, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE White       { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xFF, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Maroon      { PRIVATE_JAFG_COLOR_VALUE_CONV(0x80, 0x00, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Red         { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0x00, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Purple      { PRIVATE_JAFG_COLOR_VALUE_CONV(0x80, 0x00, 0x80, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Fuchsia     { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0x00, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Green       { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x80, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Lime        { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0xFF, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Olive       { PRIVATE_JAFG_COLOR_VALUE_CONV(0x80, 0x80, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Yellow      { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xFF, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Navy        { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x00, 0x80, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Blue        { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x00, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Teal        { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x80, 0x80, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Aqua        { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0xFF, 0xFF, 0xFF) };

//#
//# BEGIN Additional Standard
//#
inline constexpr PRIVATE_JAFG_COLOR_VALUE AliceBlue           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xF0, 0xF8, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE AntiqueWhite        { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFA, 0xEB, 0xD7, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Aqua             { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0xFF, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Aquamarine          { PRIVATE_JAFG_COLOR_VALUE_CONV(0x7F, 0xFF, 0xD4, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Azure               { PRIVATE_JAFG_COLOR_VALUE_CONV(0xF0, 0xFF, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Beige               { PRIVATE_JAFG_COLOR_VALUE_CONV(0xF5, 0xF5, 0xDC, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Bisque              { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xE4, 0xC4, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Black            { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x00, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Blanchedalmond      { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xEB, 0xCD, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Blue             { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x00, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE BlueViolet          { PRIVATE_JAFG_COLOR_VALUE_CONV(0x8A, 0x2B, 0xE2, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Brown               { PRIVATE_JAFG_COLOR_VALUE_CONV(0xA5, 0x2A, 0x2A, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE BurlyWood           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xDE, 0xB8, 0x87, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE CadetBlue           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x5F, 0x9E, 0xA0, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Chartreuse          { PRIVATE_JAFG_COLOR_VALUE_CONV(0x7F, 0xFF, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Chocolate           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xD2, 0x69, 0x1E, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Coral               { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0x7F, 0x50, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE CornflowerBlue      { PRIVATE_JAFG_COLOR_VALUE_CONV(0x64, 0x95, 0xED, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Cornsilk            { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xF8, 0xDC, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Crimson             { PRIVATE_JAFG_COLOR_VALUE_CONV(0xDC, 0x14, 0x3C, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Cyan                { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0xFF, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkBlue            { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x00, 0x8B, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkCyan            { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x8B, 0x8B, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkGoldenRod       { PRIVATE_JAFG_COLOR_VALUE_CONV(0xB8, 0x86, 0x0B, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkGray            { PRIVATE_JAFG_COLOR_VALUE_CONV(0xA9, 0xA9, 0xA9, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkGreen           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x64, 0x00, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkGrey         { PRIVATE_JAFG_COLOR_VALUE_CONV(0xA9, 0xA9, 0xA9, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkKhaki           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xBD, 0xB7, 0x6B, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkMagenta         { PRIVATE_JAFG_COLOR_VALUE_CONV(0x8B, 0x00, 0x8B, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkOliveGreen      { PRIVATE_JAFG_COLOR_VALUE_CONV(0x55, 0x6B, 0x2F, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkOrange          { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0x8C, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkOrchid          { PRIVATE_JAFG_COLOR_VALUE_CONV(0x99, 0x32, 0xCC, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkRed             { PRIVATE_JAFG_COLOR_VALUE_CONV(0x8B, 0x00, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkSalmon          { PRIVATE_JAFG_COLOR_VALUE_CONV(0xE9, 0x96, 0x7A, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkSeaGreen        { PRIVATE_JAFG_COLOR_VALUE_CONV(0x8F, 0xBC, 0x8F, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkSlateBlue       { PRIVATE_JAFG_COLOR_VALUE_CONV(0x48, 0x3D, 0x8B, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkSlateGray       { PRIVATE_JAFG_COLOR_VALUE_CONV(0x2F, 0x4F, 0x4F, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkSlateGrey    { PRIVATE_JAFG_COLOR_VALUE_CONV(0x2F, 0x4F, 0x4F, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkTurquoise       { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0xCE, 0xD1, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DarkViolet          { PRIVATE_JAFG_COLOR_VALUE_CONV(0x94, 0x00, 0xD3, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DeepPink            { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0x14, 0x93, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DeepSkyBlue         { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0xBF, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DimGray             { PRIVATE_JAFG_COLOR_VALUE_CONV(0x69, 0x69, 0x69, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE DimGrey          { PRIVATE_JAFG_COLOR_VALUE_CONV(0x69, 0x69, 0x69, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE DodgerBlue          { PRIVATE_JAFG_COLOR_VALUE_CONV(0x1E, 0x90, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE FireBrick           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xB2, 0x22, 0x22, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE FloralWhite         { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xFA, 0xF0, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE ForestGreen         { PRIVATE_JAFG_COLOR_VALUE_CONV(0x22, 0x8B, 0x22, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Fuchsia          { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0x00, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Gainsboro           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xDC, 0xDC, 0xDC, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE GhostWhite          { PRIVATE_JAFG_COLOR_VALUE_CONV(0xF8, 0xF8, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Gold                { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xD7, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Goldenrod           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xDA, 0xA5, 0x20, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Gray             { PRIVATE_JAFG_COLOR_VALUE_CONV(0x80, 0x80, 0x80, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Green            { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x80, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE GreenYellow         { PRIVATE_JAFG_COLOR_VALUE_CONV(0xAD, 0xFF, 0x2F, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Grey             { PRIVATE_JAFG_COLOR_VALUE_CONV(0x80, 0x80, 0x80, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Honeydew            { PRIVATE_JAFG_COLOR_VALUE_CONV(0xF0, 0xFF, 0xF0, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE HotPink             { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0x69, 0xB4, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Indianred           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xCD, 0x5C, 0x5C, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Indigo              { PRIVATE_JAFG_COLOR_VALUE_CONV(0x4B, 0x00, 0x82, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Ivory               { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xFF, 0xF0, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Khaki               { PRIVATE_JAFG_COLOR_VALUE_CONV(0xF0, 0xE6, 0x8C, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Lavender            { PRIVATE_JAFG_COLOR_VALUE_CONV(0xE6, 0xE6, 0xFA, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LavenderBlush       { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xF0, 0xF5, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LawnGreen           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x7C, 0xFC, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LemonChiffon        { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xFA, 0xCD, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightBlue           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xAD, 0xD8, 0xE6, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightCoral          { PRIVATE_JAFG_COLOR_VALUE_CONV(0xF0, 0x80, 0x80, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightCyan           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xE0, 0xFF, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightGoldenrodYellow{ PRIVATE_JAFG_COLOR_VALUE_CONV(0xFA, 0xFA, 0xD2, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightGray           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xD3, 0xD3, 0xD3, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightGreen          { PRIVATE_JAFG_COLOR_VALUE_CONV(0x90, 0xEE, 0x90, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE LightGrey        { PRIVATE_JAFG_COLOR_VALUE_CONV(0xD3, 0xD3, 0xD3, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightPink           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xB6, 0xC1, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightSalmon         { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xA0, 0x7A, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightSeagreen       { PRIVATE_JAFG_COLOR_VALUE_CONV(0x20, 0xB2, 0xAA, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightSkyBlue        { PRIVATE_JAFG_COLOR_VALUE_CONV(0x87, 0xCE, 0xFA, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightSlateGray      { PRIVATE_JAFG_COLOR_VALUE_CONV(0x77, 0x88, 0x99, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE LightSlateGrey   { PRIVATE_JAFG_COLOR_VALUE_CONV(0x77, 0x88, 0x99, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightSteelBlue      { PRIVATE_JAFG_COLOR_VALUE_CONV(0xB0, 0xC4, 0xDE, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LightYellow         { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xFF, 0xE0, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Lime             { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0xFF, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE LimeGreen           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x32, 0xCD, 0x32, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Linen               { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFA, 0xF0, 0xE6, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Magenta             { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0x00, 0xFF, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Maroon           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x80, 0x00, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE MediumAquaMarine    { PRIVATE_JAFG_COLOR_VALUE_CONV(0x66, 0xCD, 0xAA, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE MediumBlue          { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x00, 0xCD, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE MediumOrchid        { PRIVATE_JAFG_COLOR_VALUE_CONV(0xBA, 0x55, 0xD3, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE MediumPurple        { PRIVATE_JAFG_COLOR_VALUE_CONV(0x93, 0x70, 0xDB, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE MediumSeagreen      { PRIVATE_JAFG_COLOR_VALUE_CONV(0x3C, 0xB3, 0x71, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE MediumSlateBlue     { PRIVATE_JAFG_COLOR_VALUE_CONV(0x7B, 0x68, 0xEE, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE MediumSpringGreen   { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0xFA, 0x9A, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE MediumTurquoise     { PRIVATE_JAFG_COLOR_VALUE_CONV(0x48, 0xD1, 0xCC, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE MediumVioletRed     { PRIVATE_JAFG_COLOR_VALUE_CONV(0xC7, 0x15, 0x85, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE MidnightBlue        { PRIVATE_JAFG_COLOR_VALUE_CONV(0x19, 0x19, 0x70, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE MintCream           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xF5, 0xFF, 0xFA, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Mistyrose           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xE4, 0xE1, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Moccasin            { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xE4, 0xB5, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE NavajoWhite         { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xDE, 0xAD, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Navy             { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x00, 0x80, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Oldlace             { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFD, 0xF5, 0xE6, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Olive            { PRIVATE_JAFG_COLOR_VALUE_CONV(0x80, 0x80, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Olivedrab           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x6B, 0x8E, 0x23, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Orange              { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xA5, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Orangered           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0x45, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Orchid              { PRIVATE_JAFG_COLOR_VALUE_CONV(0xDA, 0x70, 0xD6, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE PaleGoldenRod       { PRIVATE_JAFG_COLOR_VALUE_CONV(0xEE, 0xE8, 0xAA, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE PaleGreen           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x98, 0xFB, 0x98, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE PaleTurquoise       { PRIVATE_JAFG_COLOR_VALUE_CONV(0xAF, 0xEE, 0xEE, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE PaleVioletRed       { PRIVATE_JAFG_COLOR_VALUE_CONV(0xDB, 0x70, 0x93, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Papayawhip          { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xEF, 0xD5, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Peachpuff           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xDA, 0xB9, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Peru                { PRIVATE_JAFG_COLOR_VALUE_CONV(0xCD, 0x85, 0x3F, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Pink                { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xC0, 0xCB, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Plum                { PRIVATE_JAFG_COLOR_VALUE_CONV(0xDD, 0xA0, 0xDD, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE PowderBlue          { PRIVATE_JAFG_COLOR_VALUE_CONV(0xB0, 0xE0, 0xE6, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Purple           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x80, 0x00, 0x80, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE RebeccaPurple       { PRIVATE_JAFG_COLOR_VALUE_CONV(0x66, 0x33, 0x99, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Red              { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0x00, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE RosyBrown           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xBC, 0x8F, 0x8F, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE RoyalBlue           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x41, 0x69, 0xE1, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE SaddleBrown         { PRIVATE_JAFG_COLOR_VALUE_CONV(0x8B, 0x45, 0x13, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Salmon              { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFA, 0x80, 0x72, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE SandyBrown          { PRIVATE_JAFG_COLOR_VALUE_CONV(0xF4, 0xA4, 0x60, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE SeaGreen            { PRIVATE_JAFG_COLOR_VALUE_CONV(0x2E, 0x8B, 0x57, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Seashell            { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xF5, 0xEE, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Sienna              { PRIVATE_JAFG_COLOR_VALUE_CONV(0xA0, 0x52, 0x2D, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Silver           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xC0, 0xC0, 0xC0, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE SkyBlue             { PRIVATE_JAFG_COLOR_VALUE_CONV(0x87, 0xCE, 0xEB, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE SlateBlue           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x6A, 0x5A, 0xCD, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE SlateGray           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x70, 0x80, 0x90, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE SlateGrey        { PRIVATE_JAFG_COLOR_VALUE_CONV(0x70, 0x80, 0x90, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Snow                { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xFA, 0xFA, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE SpringGreen         { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0xFF, 0x7F, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE SteelBlue           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x46, 0x82, 0xB4, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Tan                 { PRIVATE_JAFG_COLOR_VALUE_CONV(0xD2, 0xB4, 0x8C, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Teal             { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x80, 0x80, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Thistle             { PRIVATE_JAFG_COLOR_VALUE_CONV(0xD8, 0xBF, 0xD8, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Tomato              { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0x63, 0x47, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Transparent      { PRIVATE_JAFG_COLOR_VALUE_CONV(0x00, 0x00, 0x00, 0x00) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Turquoise           { PRIVATE_JAFG_COLOR_VALUE_CONV(0x40, 0xE0, 0xD0, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Violet              { PRIVATE_JAFG_COLOR_VALUE_CONV(0xEE, 0x82, 0xEE, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE Wheat               { PRIVATE_JAFG_COLOR_VALUE_CONV(0xF5, 0xDE, 0xB3, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE White            { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xFF, 0xFF, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE WhiteSmoke          { PRIVATE_JAFG_COLOR_VALUE_CONV(0xF5, 0xF5, 0xF5, 0xFF) };
// inline constexpr PRIVATE_JAFG_COLOR_VALUE Yellow           { PRIVATE_JAFG_COLOR_VALUE_CONV(0xFF, 0xFF, 0x00, 0xFF) };
inline constexpr PRIVATE_JAFG_COLOR_VALUE YellowGreen         { PRIVATE_JAFG_COLOR_VALUE_CONV(0x9A, 0xCD, 0x32, 0xFF) };

#undef PRIVATE_JAFG_COLOR_VALUE
#undef PRIVATE_JAFG_COLOR_VALUE_CONV
