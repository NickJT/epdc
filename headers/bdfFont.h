#pragma once

#include <cinttypes>

///< summary>Definition for individual glyph struct</summary>
struct BdfGlyph
{
    /// Index into the bit map array for this glyph
    uint16_t Index;
    /// Character width
    uint8_t bbw;
    /// Character height
    uint8_t bbh;
    /// Horizontal advance for glyph
    uint8_t DWidth;
    /// x Offset of glyph
    int8_t bbx;
    /// y Offset of glyph
    int8_t bby;
};

/// <summary>The global declaration for the font struct</summary>
struct BdfFont
{
    /// Start address of the bitmap data
    const uint8_t *Bitmaps;
    /// Start address of the glyph data
    const BdfGlyph *Glyphs;
    /// The first ascii character included in the font file (usually space)
    uint8_t AsciiStart;

    /// The last ascii character included in the font file (usually '~')
    uint8_t AsciiStop;
    /// The minimum number of vertical pixels needed (with zero vertical margin)
    uint8_t VerticalStep;
};

/// <summary>The vertical characteristics of the selected font. ROW_MARGIN  is added to
/// font.verticalStep to give Verticals.verticalStep which is the final row spacing </summary>
struct Verticals
{
    /// The maximum hight in pixels from origin for this font
    int8_t maxRise;
    /// The maximum drop in pixels from origin for this font
    int8_t maxDrop;
    /// The minimum vertical step for no gap or overlap
    int8_t verticalStep;
};
