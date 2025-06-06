#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include "dimensions.h"
#include "picoDatetime.h"
#include "quoteServer.h"
#include "fontServer.h"
#include "bdfFont.h"
#include "timeQuotes.h"
#include "topCat.h"
#include "frameBuffer.h"

enum class Button
{
	buttonA = 16,
	buttonB = 17,
	buttonC = 19,
};

class LayoutServer
{

public:
	LayoutServer(std::unique_ptr<displayDriver> hardwareDriver);
	void timeIs(datetime_t const t);
	void cmd(Button const b);

private:
	QuoteServer qs;
	FontServer fs;
	std::unique_ptr<displayDriver> driver;
	void renderChar(BdfGlyph glyph, std::vector<bool> v, uint_t x, uint_t y);
	size_t wrapper(std::string_view s, size_t txt, size_t len);
	std::vector<std::string_view> wordWrap(std::string_view stg, size_t len);
	size_t skipWhitespace(std::string_view s, size_t start);
	size_t setFirstCharOrigin(size_t home, char c);
	void layoutQuote(std::string_view q);
	void layoutClockFace(std::string q);
	void renderLine(std::string_view line, int originX, int originY);
};