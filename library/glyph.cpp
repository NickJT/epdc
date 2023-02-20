
#include "glyph.h"

Glyph::Glyph() : ScanLines{}, encoding{0}, dWidth{0},
				 bbx{0}, bby{0}, bbw{0}, bbh{0}
{
}

Glyph::Glyph(std::vector<std::string> tokens) : ScanLines{}, encoding{0}, dWidth{0}, bbx{0}, bby{0}, bbw{0}, bbh{0}
{
	SetDescription(tokens);
	GetBoundingBox(tokens);
	BuildGlyph(tokens);
}

void Glyph::SetDescription(std::vector<std::string> tokens)
{
	auto it{std::find(tokens.cbegin(), tokens.cend(), "STARTCHAR")};
	if (it != tokens.cend())
	{
		Name = *(it + 1);
	}

	it = std::find(tokens.cbegin(), tokens.cend(), "ENCODING");
	if (it != tokens.cend())
	{
		encoding = std::stoi(*(it + 1));
	}

	it = std::find(tokens.cbegin(), tokens.cend(), "DWIDTH");
	if (it != tokens.cend())
	{
		dWidth = std::stoi(*(it + 1));
	}
}

int Glyph::ParseInt(std::string s)
{
	return stoi(s);
}

void Glyph::GetBoundingBox(std::vector<std::string> tokens)
{
	auto it = std::find(tokens.cbegin(), tokens.cend(), "BBX");
	if (it != tokens.cend())
	{
		bbw = std::stoi(*(++it));
		bbh = std::stoi(*(++it));
		bbx = ParseInt(*(++it));
		bby = ParseInt(*(++it));
	}
}

void Glyph::BuildGlyph(std::vector<std::string> tokens)
{
	auto it{std::find(tokens.cbegin(), tokens.cend(), "BITMAP")};
	it++;
	for (uint8_t scanLine{0}; scanLine < bbh; scanLine++)
	{
		ScanLines.push_back(*it++);
	}
}

std::string Glyph::ToBitMapString() const
{
	std::stringstream ss;
	for (auto const &row : ScanLines)
	{
		unsigned i{1};
		while (i < row.length())
		{
			ss << row[i++];
			ss << row[i++] << " ";
		}
	}
	return ss.str().c_str();
}

std::string Glyph::ToString() const
{
	std::stringstream tss;
	tss
		<< bbw << ", "
		<< bbh << ", "
		<< dWidth << ", "
		<< bbx << ", "
		<< bby;
	return tss.str().c_str();
}

std::string Glyph::ToDescription() const
{
	std::stringstream tss;
	tss
		<< "// 0x"
		<< std::hex << encoding << std::dec
		<< "\t\t\t" << Name
		<< "\n";
	return tss.str().c_str();
}
