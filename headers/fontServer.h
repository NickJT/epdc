#pragma once

#include <iostream>
#include <string>
#include <cinttypes>
#include <vector>
#include "dimensions.h"
#include "geometry.h"
#include "style.h"

class FontServer
{
public:
	explicit FontServer();
	explicit FontServer(BdfFont aFont);
	// Returns verticals without the row margin (v.verticalStep = font.VerticalStep + rowMargin)
	Verticals fontVerticals();
	// Returns the glyph spec for the given character (not the data)
	// Returns the width in pixels of the given character (zero if not found)
	size_t widthOf(char const c);
	size_t widthOf(std::string stg);
	BdfGlyph glyphFor(char const c);
	// Returns data bits for the given character. Vector should contain exactly h rows of w bits
	std::vector<bool> bitsFor(char const c);

	/*
		Debug Functions
	*/
	void PrintVerticals(const Verticals verts);
	void PrintGlyph(const BdfGlyph g);
	void PrintVector(std::vector<bool> v, char const c);

private:
	BdfFont font;
	bool hasChar(char const c) const;
	uint16_t GlyphIndex(char const c);
	BdfGlyph GetGlyph(uint16_t glyphIdx);
	uint8_t RoundToByte(uint8_t value);
	std::vector<bool> Vectorize(uint8_t const *start, size_t length);
};
