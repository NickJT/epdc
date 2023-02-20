#include "fontServer.h"

#pragma region public

/**
 * @brief Construct a new Font Server with a dummy BdfFone entry
 *
 */
FontServer::FontServer()
{
	BdfFont dummyFont = {
		.Bitmaps = nullptr,
		.Glyphs = nullptr,
		.AsciiStart = ' ',
		.AsciiStop = ' ',
		.VerticalStep = 0};

	font = dummyFont;
}

FontServer::FontServer(BdfFont aFont) : font{aFont}
{
}

/// <summary>Sets the vertical extents needed for layout from the passed font</summary>
/// <param name= font>The font to examine </param>
/// <returns>Verticals A completed struct with the rise, drop and vertical step values</returns>
Verticals FontServer::fontVerticals()
{
	// Get the extent of the font
	uint16_t glyphCount = font.AsciiStop - font.AsciiStart;
	Verticals v;
	// Rise is always positive but is calculated from a uint8_t and a int8_t
	v.maxRise = 0;
	// Drop can be zero or negative (uses int8_t)
	v.maxDrop = 0xFF;
	// v.verticalStep = font.VerticalStep + rowMargin
	v.verticalStep = font.VerticalStep;
	for (uint16_t i{0}; i < glyphCount; i++)
	{
		// GetGlyph returns a value in RAM that we can use directly
		BdfGlyph g{GetGlyph(i)};
		int8_t rise = g.bbh + g.bby;
		v.maxRise = (rise > v.maxRise) ? rise : v.maxRise;
		v.maxDrop = (g.bby < v.maxDrop) ? g.bby : v.maxDrop;
	}
	// PrintVerticals(v);
	if (v.maxRise > v.verticalStep)
	{
		printf("Error in Extent calculation\n\r\n\r");
	}
	return v;
}

size_t FontServer::widthOf(char const c)
{
	char chr{hasChar(c) ? c : ERROR_CHAR};
	return font.Glyphs[GlyphIndex(chr)].DWidth;
}

size_t FontServer::widthOf(std::string stg)
{
	size_t totalWidth{0};
	for (auto const &c : stg)
	{
		totalWidth += widthOf(c);
	}
	return totalWidth;
}

/// @brief Returns the glyph specification for the passed character
/// @param c - The character we want the glyph for
/// @return - The glyph spec for c (or for "#" is c is not found)
BdfGlyph FontServer::glyphFor(char const c)
{
	char chr{hasChar(c) ? c : ERROR_CHAR};
	return GetGlyph(GlyphIndex(chr));
}

// Returns a vector<bool> containing the bits comprising the character c
std::vector<bool> FontServer::bitsFor(char const c)
{
	char chr{hasChar(c) ? c : ERROR_CHAR};

	auto g{glyphFor(chr)};
	uint8_t const *start{font.Bitmaps + g.Index};
	auto widthInBytes{RoundToByte(g.bbw)};
	size_t lengthInBytes{widthInBytes * static_cast<size_t>(g.bbh)};

	return Vectorize(start, lengthInBytes);
}

#pragma endregion

#pragma region private

/// <summary>Returns true is a given char (uint8_t) is within the range of a font</summary>
/// Fonts must have consecutive ascii glyph values so the check is based on value range not matching
/// <param name= font>The font whose range is to be checked</param>
/// <param name= chr>The character to check for</param>
/// <returns>true If the font contains a glyph representing the given char</returns>
/// <returns>false If the character is out of range </returns>
bool FontServer::hasChar(char const c) const
{
	return !((c < font.AsciiStart) || (c > font.AsciiStop));
}

/// <summary>Cacluates the numerical index into the Glyph array</summary>
/// <param name="c">The character we want</param>
/// <returns>The index of c as a uint16_t</returns>
uint16_t FontServer::GlyphIndex(char const c)
{
	return (c - ' ');
}

/// <summary>Gets a glyph from program memory using the glyph's index</summary>
/// <param name= font>A const ref to a BdfFont struct</param>
/// <param name= glyphIdx>The index of the glyph where the first glyph has index zero</param>
/// <returns>BdfGlyph The glyph at glyphIdx</returns>
BdfGlyph FontServer::GetGlyph(uint16_t glyphIdx)
{
	const BdfGlyph *glyph{font.Glyphs + glyphIdx};
	BdfGlyph result;
	result.Index = (glyph->Index);
	result.bbw = (glyph->bbw);
	result.bbh = (glyph->bbh);
	result.DWidth = (glyph->DWidth);
	result.bbx = (glyph->bbx);
	result.bby = (glyph->bby);
	return result;
}

/// <summary>Rounds a value to the next highest multiple of eight</summary>
/// <param name= value>The value to be rounded</param>
/// <returns>uint8_t A uint8_t no lower than value and which is divisible by eight</returns>
uint8_t FontServer::RoundToByte(uint8_t value)
{
	return (value + 7) / 8;
}

// Returns a vector<bool> containing length bits starting at the address given by start
std::vector<bool> FontServer::Vectorize(uint8_t const *start, size_t length)
{
	auto result{std::vector<bool>()};
	if (start == nullptr || length == 0)
	{
		return result;
	}

	for (size_t i{0}; i < length; i++)
	{
		uint8_t b{*(start + i)};
		uint8_t mask{0b10000000};
		for (uint8_t shift{0}; shift < 8; shift++)
		{
			result.push_back(b & mask);
			mask = mask >> 1;
		}
	}
	return result;
}

#pragma endregion

#pragma region debug functions

/// <summary>Debug-only procedure to check verticals</summary>
/// <param name= verts>The Verticals struct to print</param>
void FontServer::PrintVerticals(const Verticals verts)
{
	printf("\n\rMax rise = \t %i \n\r", verts.maxRise);
	printf("\n\rMax drop = \t %i \n\r", verts.maxDrop);
	printf("\n\rV Step = \t %i \n\r", verts.verticalStep);
}

/// <summary>Debug-only function to print the parameters of the passed glyph</summary>
/// The glyph needs to be in RAM for printing so precede this with a call to
/// GetGlyph for glyphs in ProgMem
/// <param name= g>The glyph to be printed</param>
void FontServer::PrintGlyph(const BdfGlyph g)
{
	printf("Index: \t %i\n\r", g.Index);
	printf("bbw: \t %i \n\r", g.bbw);
	printf("bbh: \t %i \n\r", g.bbh);
	printf("DWidth:\t %i \n\r", g.DWidth);
	printf("bbx: \t %i\n\r", g.bbx);
	printf("bby: \t %i\n\r", g.bby);
}

/// @brief Debug-only function to print the bitmap bit in the passed vector according to the
/// glyph specifications of the passed character
/// @param v - The vector of bits
/// @param c - The character that the bits should represent when interpreted according to the glyph spec
void FontServer::PrintVector(std::vector<bool> v, char const c)
{
	auto g{glyphFor(c)};
	size_t widthInBytes{RoundToByte(g.bbw)};
	size_t widthInBits{widthInBytes * 8};
	size_t totalBits{widthInBytes * g.bbh * 8};
	if (totalBits != v.size())
	{
		std::cout << "Bits expected: " << totalBits << " ";
		std::cout << "Bits found " << v.size() << std::endl;
	}
	size_t col{0};
	std::cout << std::endl;
	for (auto b : v)
	{
		if (b)
		{
			std::cout << 'X';
		}
		else
		{
			std::cout << ' ';
		}
		col++;
		if (col == widthInBits)
		{
			std::cout << std::endl;
			col = 0;
		}
	}
}

#pragma endregion