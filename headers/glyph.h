#pragma once

#ifndef GLYPH_H
#define GLYPH_H

#include <string>
#include <vector>
#include <sstream>
#include <string>
#include <cctype>
#include <algorithm>
#include <cstdint>

class Glyph
{
public:
	Glyph();
	Glyph(std::vector<std::string> tokens);

	void GetBoundingBox(std::vector<std::string> tokens);
	void SetDescription(std::vector<std::string> tokens);
	int ParseInt(std::string s);
	void BuildGlyph(std::vector<std::string> tokens);

	std::vector<std::string> ScanLines;
	std::string Name;
	int encoding;
	int dWidth;
	int bbx;
	int bby;
	int bbw;
	int bbh;

	std::string ToString() const;
	std::string ToBitMapString() const;
	std::string ToDescription() const;

};

#endif