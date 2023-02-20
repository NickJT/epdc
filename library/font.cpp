#include <climits>
#include "font.h"

using std::string;
using std::stringstream;

const string Font::Definitions()
{
	stringstream ss;
	ss << "\n"
	   << "#pragma once\n"
	   << "\n#include <cinttypes>\n"
	   << "\n";

	ss << GlyphDeclaration();
	ss << FontDeclaration();

	ss << "\n\n";

	return ss.str().c_str();
}

Font::Font() : glyphs{}
{
	Name = "Empty";
	tag_ = "None";
	vStep = 0;
	Width = 0;
	Height = 0;
	xOffset = 0;
	yOffset = 0;
	TotalGlyphs = 0;
	minEncoding_ = 32;
	maxEncoding_ = 126;
	bbhMax = INT_MIN;
	bbhMin = INT_MAX;
	bbwMax = INT_MIN;
	bbwMin = INT_MAX;
	bbxMax = INT_MIN;
	bbxMin = INT_MAX;
	bbyMax = INT_MIN;
	bbyMin = INT_MAX;
	dWidthMax = INT_MIN;
	dWidthMin = INT_MAX;
	vExtentMax = INT_MIN;
}

void Font::LoadFont(std::vector<string> tokens)
{
	auto it{std::find(tokens.cbegin(), tokens.cend(), "FONT")};
	auto NameEnd{std::find(tokens.cbegin(), tokens.cend(), "SIZE")};

	stringstream ss{};

	it++;
	if (it != tokens.cend() && NameEnd != tokens.cend())
	{
		while (it != NameEnd)
		{
			ss << *(it++);
		}
		Name = ss.str();
	}

	it = std::find(tokens.cbegin(), tokens.cend(), "FONTBOUNDINGBOX");
	if (it != tokens.cend())
	{
		Width = ParseInt(*(++it));
		Height = ParseInt(*(++it));

		if (Width <= 0 || Height <= 0)
		{
			//("Unknown character size\n");
		}

		// The next two values could be +ve or -ve
		xOffset = ParseInt(*(++it));
		yOffset = ParseInt(*(++it));
	}

	it = std::find(tokens.cbegin(), tokens.cend(), "CHARS");
	if (it != tokens.cend())
	{
		TotalGlyphs = std::stoi((*(it + 1)));
	}

	tokens.erase(tokens.begin(), it + 2);
	LoadGlyphs(tokens);
	Extents();
}

int Font::ParseInt(std::string s)
{
	return stoi(s);
}

void Font::LoadGlyphs(std::vector<string> tokens)
{
	while ((tokens.size() > 0) && (tokens.at(0) != "ENDFONT"))
	{
		std::vector<string> glypstrings{};
		auto endIt = std::find(tokens.cbegin(), tokens.cend(), "ENDCHAR");
		if (endIt == tokens.cend())
		{
			tokens.clear();
			return;
		}
		auto it{tokens.cbegin()};
		for (; it != endIt; it++)
		{
			glypstrings.push_back(*it);
		};
		glypstrings.push_back(*(it));

		auto nextGlyph{Glyph(glypstrings)};

		if ((nextGlyph.encoding >= minEncoding_) && (nextGlyph.encoding <= maxEncoding_))
		{
			glyphs.push_back(nextGlyph);
		}

		tokens.erase(tokens.begin(), it + 1);
		// Limit input to the first printable characters
		if ((nextGlyph.encoding > maxEncoding_) || (nextGlyph.encoding < 0) || (glyphs.size() == static_cast<size_t>(TotalGlyphs)))
		{
			break;
		}
	}
}

void Font::Extents()
{
	for (auto const &glyph : glyphs)
	{
		bbhMax = (glyph.bbh > bbhMax) ? glyph.bbh : bbhMax;
		bbwMax = (glyph.bbw > bbwMax) ? glyph.bbw : bbwMax;
		bbxMax = (glyph.bbx > bbxMax) ? glyph.bbx : bbxMax;
		bbyMax = (glyph.bby > bbyMax) ? glyph.bby : bbyMax;
		dWidthMax = (glyph.dWidth > dWidthMax) ? glyph.dWidth : dWidthMax;

		bbhMin = (glyph.bbh < bbhMin) ? glyph.bbh : bbhMin;
		bbwMin = (glyph.bbw < bbwMin) ? glyph.bbw : bbwMin;
		bbxMin = (glyph.bbx < bbxMin) ? glyph.bbx : bbxMin;
		bbyMin = (glyph.bby < bbyMin) ? glyph.bby : bbyMin;
		dWidthMin = (glyph.dWidth < dWidthMin) ? glyph.dWidth : dWidthMin;

		int vExtent{glyph.bbh + glyph.bby};
		vExtentMax = (vExtent > vExtentMax) ? vExtent : vExtentMax;
	}
	vStep = vExtentMax - bbyMin;
}

const unsigned int Font::RoundToByte(unsigned int value) const
{
	return ((value + 7) >> 3);
}

string Font::GenerateBitmaps()
{
	int maxLineLength{50};
	int cursorPos{0};
	stringstream ss;
	for (auto const &glyph : glyphs)
	{
		for (auto const &row : glyph.ScanLines)
		{
			unsigned i{0};
			while (i < row.length())
			{
				ss << "0x" << row[i++];
				ss << row[i++] << ", ";
				cursorPos += 6;
				if (cursorPos >= maxLineLength)
				{
					ss << "\n";
					cursorPos = 0;
				}
			}
		}
	}
	ss << "};\n\n";
	return ss.str().c_str();
}

const string Font::BitmapDef() const
{
	int maxLineLength{50};
	int cursorPos{0};

	stringstream ss;
	ss << "\nconstexpr uint8_t " << tag_.c_str() << "Bitmaps[] {\n";

	for (auto const &glyph : glyphs)
	{
		for (auto const &row : glyph.ScanLines)
		{
			unsigned i{0};
			while (i < row.length())
			{
				ss << "0x" << row[i++];
				ss << row[i++] << ", ";
				cursorPos += 6;
				if (cursorPos >= maxLineLength)
				{
					ss << "\n";
					cursorPos = 0;
				}
			}
		}
	}
	ss << "};\n\n";
	return ss.str().c_str();
}

string Font::GlyphComment()
{
	stringstream ss;
	ss
		<< "\n/*\nBdfGlyph\n"
		<< "Index - \tIndex into the bit map array for this glyph\n"
		<< "bbw - \t\tCharacter width\n"
		<< "bbh - \t\tCharacter height\n"
		<< "DWidth - \tHorizontal advance for glyph\n"
		<< "bbx - \t\t\tHorizontal offset of glyph\n"
		<< "bby - \t\t\tVertical offset of glyph\n*/";
	return ss.str().c_str();
}

const string Font::GlyphSpecDef() const
{
	stringstream ss;
	ss << "\n constexpr BdfGlyph " << tag_.c_str() << "Glyphs[] {\n";

	unsigned int index{0};
	for (auto const &glyph : glyphs)
	{
		ss << "{"
		   << index << ", "
		   << glyph.ToString()
		   << "},"
		   << "\t\t\t"
		   << glyph.ToDescription();
		index += (RoundToByte(glyph.bbw) * glyph.bbh);
	}
	ss << "};\n\n";
	return ss.str().c_str();
}

string Font::GlyphDeclaration()
{
	stringstream ss;
	ss
		<< "\n\n///<summary>Definition for individual glyph struct</summary>\n"
		<< "struct BdfGlyph\n"
		<< "{\n"
		<< "\t/// Index into the bit map array for this glyph\n"
		<< "\tuint16_t Index;\t\t\n"
		<< "\t/// Character width\n"
		<< "\tuint8_t bbw;\n"
		<< "\t/// Character height\n"
		<< "\tuint8_t bbh;\n"
		<< "\t/// Horizontal advance for glyph\n"
		<< "\tuint8_t DWidth;\n"
		<< "\t/// x Offset of glyph\n"
		<< "\tint8_t bbx;\n"
		<< "\t/// y Offset of glyph\n"
		<< "\tint8_t bby;\n"
		<< "};";
	return ss.str().c_str();
}

string Font::FontDeclaration()
{
	stringstream ss;
	ss << "\n\n/// <summary>The global declaration for the font struct</summary>"
	   << "\nstruct BdfFont\n"
	   << "{"
	   << "\n\t/// Start address of the bitmap data"
	   << "\n\tconst uint8_t* Bitmaps;"
	   << "\n\t/// Start address of the glyph data"
	   << "\n\tconst BdfGlyph* Glyphs;"
	   << "\n\t/// The first ascii character included in the font file (usually space)"
	   << "\n\tuint8_t AsciiStart;"
	   << "\n\t/// The last ascii character included in the font file (usually '~')"
	   << "\n\tuint8_t AsciiStop;"
	   << "\n\t/// The minimum number of vertical pixels needed (with zero vertical margin)"
	   << "\n\tuint8_t VerticalStep;"
	   << "\n};\n";
	return ss.str().c_str();
}

const string Font::FontSummary() const
{
	stringstream ss;
	ss << "/****** Font Summary ********";
	ss << ToSummary();
	ss << "\n****************************/\n";
	return ss.str().c_str();
}

const string Font::FontDefinitionBlock() const
{

	stringstream ss;
	ss << "\n/****** Font Definition ******/";
	ss << FontDefinition();
	ss << "/****************************/\n";
	return ss.str().c_str();
}

const string Font::FontDefinition() const
{
	stringstream ss;
	ss << "\n\nconst BdfFont "
	   << tag_.c_str() << " \n{\n"
	   << "\t///Start address for bitmaps\n"
	   << "\t" << tag_.c_str() << "Bitmaps,\n"
	   << "\t///Start address for glyphs\n"
	   << "\t" << tag_.c_str() << "Glyphs, \n"
	   << "\t///Lowest ASCII encoding\n"
	   << "\t" << glyphs.at(0).encoding << ",\n"
	   << "\t///Highest ASCII encoding\n"
	   << "\t" << glyphs.at(glyphs.size() - 1).encoding << ",\n"
	   << "\t/// Vertical step\n"
	   << "\t" << vStep
	   << "\n};\n\n";
	return ss.str().c_str();
}

const string Font::ToSummary() const
{
	int first{glyphs.at(0).encoding};
	char firstChar{static_cast<char>(first)};
	int last{glyphs.at(glyphs.size() - 1).encoding};
	char lastChar{static_cast<char>(last)};
	int gaps{(last - first + 1) - static_cast<int>(glyphs.size())};

	stringstream ss;
	ss << "\nTag\t\t\t" << Tag().c_str()
	   << "\nWidth\t\t" << Width
	   << "\nHeight\t\t" << Height
	   << "\nX:Y Offset\t" << xOffset << " : " << yOffset
	   << "\nGlyphs\t\t" << TotalGlyphs
	   << "\n\nGlyphs Extracted"
	   << "\nGlyphs\t" << glyphs.size()
	   << "\nFirst\t" << first << "\t(" << firstChar << ")"
	   << "\nLast\t" << last << "\t(" << lastChar << ")"
	   << "\nbbw\t\t" << bbwMin << " to " << bbwMax
	   << "\nbbh\t\t" << bbhMin << " to " << bbhMax
	   << "\nbbx\t\t" << bbxMin << " to " << bbxMax
	   << "\nbby\t\t" << bbyMin << " to " << bbyMax
	   << "\nDWidth\t" << dWidthMin << " to " << dWidthMax
	   << "\nMax Rise " << vExtentMax
	   << "\nMax Drop " << bbyMin
	   << "\nV Step\t" << vStep;

	if (gaps > 0)
	{
		ss << "\n\tWarning: Encodings may not be contiguous (" << gaps << " gaps)";
	}
	else if (gaps < 0)
	{
		ss << "\n\tWarning: Possible duplicate encodings";
	}

	return ss.str().c_str();
}

const string Font::Guards() const
{
	string guard{Tag()};
	for (unsigned i = 0; i < guard.length(); i++)
	{
		guard[i] = toupper(guard[i]);
	}
	stringstream ss;
	ss << "\n"
	   << "#pragma once\n"
	   << "\n#include <cinttypes>\n"
	   << "#include \"bdfFont.h\"\n"
	   << "\n";
	return ss.str().c_str();
}

const string Font::ToIncludeFile() const
{
	stringstream ss{};
	ss
		<< Guards()
		<< FontSummary()
		<< GlyphSpecDef()
		<< BitmapDef()
		<< FontDefinitionBlock()
		<< "\n";
	return ss.str();
}

const std::string Font::Tag() const
{
	return tag_;
}

void Font::Tag(std::string tag)
{
	tag.erase(std::remove_if(
				  tag.begin(), tag.end(),
				  [](char c)
				  { return c == ' ' || c == '-'; }),
			  tag.end());
	tag_ = tag;
}

const int Font::GlyphCount() const
{
	return glyphs.size();
}
