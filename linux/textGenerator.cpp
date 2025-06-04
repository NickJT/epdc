/**
 * @file textGenerator.cpp
 * @author Nick (nickjt@duck.com)
 * @brief Ingests a tab separated values file in the following format
 * hh:mm:nn	Text	Title	Author
 * Writes an intermediate file containing a version of the above with characters
 * limited to  * ' ' > char < '~'
 * Writes an include file in the folowing format
 * n	Text
 * where n = hh * 60 + mm
 * Definitions for input and output files and directors are directly below this header
 * ** WARNING - Currently being converted from a very hacky prototype *****
 * @version 0.1
 * @date 2023-01-10
 *
 * @copyright Copyright (c) 2023
 *
 */

constexpr auto IN_DIRECTORY{"/home/nick/Documents"};
constexpr auto INPUT_FILE{"text"};
constexpr auto INPUT_EXT{"tsv"};

constexpr auto OUT_DIRECTORY{"/home/nick/Documents"};
constexpr auto CLEANED_FILE{"items"};
constexpr auto OUTPUT_EXT{"tsv"};

constexpr auto INCLUDE_DIRECTORY{"/home/nick/Documents"};
constexpr auto INCLUDE_FILE{"timeQuotes"};
constexpr auto INCLUDE_EXT{"h"};

constexpr char TAB{static_cast<char>(0x09)};
constexpr char LF{static_cast<char>(0x0A)};
constexpr char CR{static_cast<char>(0x0D)};
//constexpr char EOT{static_cast<char>(0x00)};

#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <ctype.h>
#include <map>

#include "../headers/picoDatetime.h"
#include "../headers/asset.h"

struct timeText
{
	std::string timeString;
	datetime_t dt;
	std::string marker;
	std::string text;
	std::string source;
	std::string author;
};

uint16_t hashTime(datetime_t dt)
{
	return dt.hour * 60 + dt.min;
}

void replaceDoubleQuotes(std::string &raw)
{
	std::replace(raw.begin(), raw.end(), '"', '\'');
}

void removeBreaks(std::string &raw)
{
	// Windows: CR and LF Linux: LF
	raw.erase(std::remove_if(raw.begin(), raw.end(),
							 [](char c)
							 { return c == CR || c == LF; }),
			  raw.end());
	return;
}

bool cleanTimeString(std::vector<timeText> &items)
{
	for (auto &item : items)
	{
		std::replace(item.timeString.begin(), item.timeString.end(), '.', ':');
		if (std::count(item.timeString.begin(), item.timeString.end(), ':') != 2)
		{
			std::cout << "Bad time string: " << item.timeString << std::endl;
			return false;
		}
	}
	return true;
}

bool cleanText(std::vector<timeText> &items)
{
	std::map<uint8_t, char> badChars;
	badChars[128] = 'C';
	badChars[130] = 'e';
	badChars[131] = 'a';
	badChars[132] = 'a';
	badChars[133] = 'a';
	badChars[135] = 'c';
	badChars[136] = 'e';
	badChars[137] = 'e';
	badChars[138] = 'e';
	badChars[139] = 'i';
	badChars[140] = 'i';
	badChars[141] = 'i';
	badChars[142] = 'A';
	badChars[143] = 'A';
	badChars[144] = 'E';
	badChars[146] = '\'';
	badChars[147] = 'o';
	badChars[148] = 'o';
	badChars[149] = 'o';
	badChars[150] = 'u';
	badChars[151] = 'u';
	badChars[152] = 'y';
	badChars[153] = '\'';
	badChars[160] = 'a';
	badChars[161] = 'i';
	badChars[163] = 'u';
	badChars[162] = 'o';
	badChars[163] = 'u';
	badChars[164] = 'n';
	badChars[166] = '-';
	badChars[195] = 'e';
	badChars[169] = '\'';
	badChars[170] = '\'';
	badChars[226] = '\'';

	bool bad{false};
	int row {0};
	for (auto &item : items)
	{
		row++;
		for (size_t i{0}; i < item.text.size(); i++)
		{
			char c{item.text.at(i)};
			//std::cout << c << " " ;
			if ((c < ' ' || c > '~') && c != CR && c != LF)
			{
				if (badChars.contains((uint8_t)c))
				{
					item.text.at(i) = badChars.at(c);
					std::cout << "Row " << row  << " Bad " << c << " (" << (int) c << ") replaced with " << item.text.at(i) << std::endl;
				}
				else
				{
					item.text.at(i) = ' ';
					std::cout << "Row " << row  << " " << " Unknown: " << c << " (" << (int) c << ") replaced with space" << std::endl;
					bad = true;
				}
			}
		}
	}
	return !bad;
}

/// <summary> Generates the definitions of the Asset struct and the AssetStack struct</summary>
/// <returns>A std::string with the definition text</returns>
std::string assetDeclarations()
{
	std::stringstream ss;
	ss << "#pragma once\n"
	   << "#include \"asset.h\"\n"
	   << "/* \n"
	   << "#include <cinttypes>\n\n"
	   << "#include <span>\n\n"
	   << "/// <summary>Declaration for the Asset struct used to retrieve and format the asset text</summary>\n"
	   << "struct Asset\n{\n"
	   << "\t///Space for a 16-bit key (currently holds the zero-based ordinal number of the asset)\n"
	   << "\tsize_t Key;\n"
	   << "\t///Byte index into the asset array for this asset\n"
	   << "\tsize_t Index;\n"
	   << "\t///A code for the required font (not used yet)\n"
	   << "\tuint8_t FontId;\n"
	   << "\t///A code for the required style (not used yet)\n"
	   << "\tuint8_t Style;\n"
	   << "};\n"
	   << "\n"
	   << "/// <summary>Declaration for the struct describing the overall stack of quotes</summary>\n"
	   << "struct AssetStack\n"
	   << "{\n"
	   << "\t///The const char array containing the asset text. Not a string_view because we have \n"
	   << "\t///multiple EOT characters in the block\n"
	   << "\tstd::span<const char> Text;\n"
	   << "\t///A pointer to the first Asset struct in the array containing the asset details\n"
	   << "\tstd::span<const Asset> Assets;\n"
	   << "\t///The total number of quotes in this stack\n"
	   << "\tconst size_t Quantity;\n"
	   << "\t///The size of the text table in bytes\n"
	   << "\tconst size_t TextSize;\n"
	   << "};"
	   << "\n */\n\n";

	return ss.str();
}

/// <summary> Generates the closing endif statement for the include file</summary>
/// <returns>The endif string</returns>
std::string endifInclude()
{
	return "\n\n#endif\n\n\n";
}

/// <summary>Generates the definition of the array of asset structs</summary>
/// <param name="qDefs">A std::vector<Asset> containing the details of the quotes included in generated include file</param>
/// <returns></returns>
std::string assetDetails(const std::vector<Asset> &qDefs)
{
	std::stringstream ds;
	ds << "\n///Definition of the array of Asset structs\n";

	ds << "const Asset timeAssets[] \n{\n";
	for (auto const &q : qDefs)
	{
		ds << "\t{" << (int)q.Key << ", " << (int)q.Index
		   << ", " << (int)q.FontId << ", " << (int)q.Style
		   << "},\n";
	}
	ds << "};\n\n";

	return ds.str();
}

/// <summary> Generates the definitions for the asset structs </summary>
/// <param name="quotes">A std::vector<std::string> containing quotes with NewLine characters '\n' making the line endings</param>
/// <returns>The defitions followed by the definition of the overall asset stack to use
/// in the calling application</returns>
std::string assetDefinitions(const std::vector<std::string> &quotes)
{
	using namespace std;
	constexpr int MaxIncludeFileLineLength{50};
	constexpr char NewLine('\n');
	vector<Asset> qDefs;
	stringstream ss;
	uint32_t index{0};
	int cursorPos{0};
	uint16_t key{0};

	ss << "\n///Definition of the array of chars forming the Assets\n";
	ss << "const char timeText[]  \n{\n\t";

	for (auto const &asset : quotes)
	{
		Asset qDef{0, 1, 0, 0};
		qDef.Index = index;
		qDef.Key = key;
		ss << "/*Key: " << qDef.Key << " Index: " << qDef.Index << " */"
		   << "n\t";
		key++;
		for (auto const c : asset)
		{
			if (c == NewLine)
			{
				ss << "0x0A, ";
			}
			else
			{
				ss << "0x" << hex << (int)c << ", ";
			}
			index++;
			cursorPos += 6;
			if (cursorPos >= MaxIncludeFileLineLength)
			{
				ss << "\n\t";
				cursorPos = 0;
			}
		}
		ss << "0x00, ";
		index++;
		ss << "\n\n\t";
		cursorPos = 0;
		qDefs.push_back(qDef);
	}

	ss << "\n";
	cursorPos = 0;
	// We increment index one last time to give the size
	index++;

	ss << "};\n";

	ss << assetDetails(qDefs);

	ss << "\n/* AssetStack definition to add to application  */\n";
	ss << "/* AssetStack stack{ timeText, timeAssets, " << dec << qDefs.size() << ", " << (int)index << "}; */\n\n";

	return ss.str();
}

/// <summary> Generates the definitions for the asset structs </summary>
/// <param name="quotes">A vector<std::pair<int, std::string>> containing quotes and index numbers</param>
/// <returns>The defitions followed by the definition of the overall asset stack to use
/// in the calling application</returns>
std::string assetAndIndexDefinitions(std::vector<timeText> const &items)
{
	using namespace std;
	constexpr int MaxIncludeFileLineLength{50};
	constexpr char NewLine('\n');
	vector<Asset> qDefs;
	stringstream ss;
	uint32_t index{0};
	int cursorPos{0};

	ss << "\n///Definition of the array of chars forming the Assets\n";
	ss << "const char timeText[]  \n{\n\t";

	for (auto const &item : items)
	{
		Asset qDef{0, 1, 0, 0};
		qDef.Index = index;
		qDef.Key = hashTime(item.dt);
		ss << "/*Key: " << std::dec << qDef.Key << " Index: " << std::dec << qDef.Index << std::hex << " */"
		   << "\n\t";
		for (auto const c : item.text)
		{
			if (c == NewLine)
			{
				ss << "0x0A, ";
			}
			else
			{
				ss << "0x" << hex << static_cast<int>(c) << ", ";
			}
			index++;
			cursorPos += 6;
			if (cursorPos >= MaxIncludeFileLineLength)
			{
				ss << "\n\t";
				cursorPos = 0;
			}
		}
		ss << "0x00, ";
		index++;
		ss << "\n\n\t";
		cursorPos = 0;
		qDefs.push_back(qDef);
	}

	ss << "\n";
	cursorPos = 0;

	ss << "};\n";

	ss << assetDetails(qDefs);

	ss << "\n/* AssetStack definition to add to application  */\n";
	ss << "/* AssetStack stack{ timeText, timeAssets, " << dec << qDefs.size() << ", " << (int)index << "}; */\n\n";

	return ss.str();
}

/// <summary> Generates the text string needed for an include file contining the asset data and the struct used for indexing
/// <param name="quotes">A std::vector<std::string> containing quotes with NewLine characters '\n' making the line endings</param>
/// <returns>A std::string containing all of the text needed for the include file</returns>
std::string makeInclude(const std::vector<timeText> &items)
{
	std::stringstream ss;

	ss << assetDeclarations();
	ss << assetAndIndexDefinitions(items);

	return ss.str();
}

std::filesystem::path makePath(const std::string dir, const std::string stem, const ::std::string ext)
{
	std::filesystem::path outPath{dir};
	outPath.make_preferred();
	outPath += std::filesystem::path::preferred_separator;
	outPath += stem + "." + ext;
	return outPath;
}

bool writeFile(std::string const &s, std::filesystem::path p)
{
	std::ofstream oFile(p);
	if (!oFile)
	{
		return false;
	}

	oFile << s << std::endl;
	oFile.close();

	return true;
}

std::vector<std::string> makeLines(std::string const &raw, char delimiter)
{
	std::vector<std::string> lines;
	std::istringstream ss(raw);
	while (!ss.eof())
	{
		std::string line;
		getline(ss, line, delimiter);
		removeBreaks(line);
		if (!line.empty())
		{
			lines.push_back(line);
		}
	}
	return lines;
}

std::vector<timeText> makeItems(std::vector<std::string> lines)
{
	std::vector<timeText> items;
	for (auto const &line : lines)
	{
		std::istringstream ss(line);
		std::vector<std::string> fields;
		while (!ss.eof())
		{
			std::string field;
			getline(ss, field, TAB);
			fields.push_back(field);
		}

		if (fields.size() != 5)
		{
			std::cout << "Error - Length = " << fields.size() << std::endl;
			std::cout << "Line: " << line << std::endl;
			return items;
		}

		timeText t{
			.timeString = fields.at(0),
			.dt = datetime_t{0, 0, 0, 0, 0, 0, 0},
			.marker = fields.at(1), 
			.text = fields.at(2),
			.source = fields.at(3),
			.author = fields.at(4),
		};
		items.push_back(t);
	}
	return items;
}

void ShowTimeStringError(datetime_t dt)
{
	std::cout << "Timestring error " << dt.hour << ":" << dt.min << ":" << dt.sec << std::endl;
}

bool makeDatetimes(std::vector<timeText> &items)
{
	size_t result;
	for (auto &item : items)
	{ // hh:mm:ss
		item.dt.hour = std::stoi(item.timeString.substr(0, 2), &result);
		if (result != 2)
		{
			ShowTimeStringError(item.dt);
		}
		item.dt.min = std::stoi(item.timeString.substr(3, 2), &result);
		if (result != 2)
		{
			ShowTimeStringError(item.dt);
		}
		item.dt.sec = std::stoi(item.timeString.substr(6, 2), &result);
		if (result != 2)
		{
			ShowTimeStringError(item.dt);
		}

		if (item.dt.hour > 59 || item.dt.min > 59 || item.dt.sec > 59 ||
			item.dt.hour < 0 || item.dt.min < 0 || item.dt.sec < 0)
		{
			std::cout << "Error in datetime creation" << std::endl;
			ShowTimeStringError(item.dt);
			return false;
		}
	}
	return true;
}

void checkMarkers(std::vector<timeText> &items) {
	auto tally {0};
	for (auto &item : items) {
		auto mkr {item.marker};
		std::transform(mkr.begin(), mkr.end(), mkr.begin(), ::toupper);
		auto txt {item.text};
		std::transform(txt.begin(), txt.end(), txt.begin(), ::toupper);
		if (txt.find(mkr) == std::string::npos)
		{
			std::cout << "[" << item.marker << "] not in " << item.text << std::endl;
			tally++;
		}
	}
	std::cout << "\n" << tally << " mismatched markers found" << std::endl;
}

std::string GetFileContent(std::filesystem::path fPath)
{
	std::ifstream inFile(fPath.make_preferred());
	std::stringstream ss;

	if (inFile.is_open())
	{
		ss << inFile.rdbuf();
		inFile.close();
	}
	return ss.str();
}

bool writeCleanedFile(std::vector<timeText> v, std::filesystem::path p)
{
	std::ofstream oFile(p);
	if (!oFile)
	{
		return false;
	}

	for (auto item : v)
	{
		oFile << std::setw(2) << std::setfill('0')
			  << static_cast<int>(item.dt.hour) << ":"
			  << std::setw(2) << std::setfill('0')
			  << static_cast<int>(item.dt.min) << ":"
			  << "00" << TAB
			  << item.text << TAB
			  << item.source << TAB
			  << item.author << CR << LF;
	}
	oFile << std::endl;
	oFile.close();

	return true;
}

/* Debug only */
void checkLengths(std::vector<timeText> items)
{
	for (auto const &item : items)
	{
		std::cout << item.timeString << " - " << item.text.size() << "\n\r";
	}
	std::cout << std::endl;
}

int main()
{
	using namespace std;
	std::cout << std::endl;
	std::cout << "Ingester\n\r";

	auto path{makePath(IN_DIRECTORY, INPUT_FILE, INPUT_EXT)};
	std::cout << "Reading: \t" << path << "\n\r";
	string raw{GetFileContent(path)};
	if (raw.empty())
	{
		std::cout << "Input file (" << path << ") was empty" << std::endl;
		exit(EXIT_FAILURE);
	}

	replaceDoubleQuotes(raw);
	auto lines{makeLines(raw, LF)};
	auto items{makeItems(lines)};
	
	checkMarkers(items);

	std::cout << TAB << TAB << items.size() << " items found\n\r";

	if (!cleanText(items))
	{
		std::cout << "\t\tExtended ascii characters found\n\r";
	}
	if (!cleanTimeString(items))
	{
		std::cout << "\t\tTime strings needed cleaning\n\r";
	}

	if (!makeDatetimes(items))
	{
		std::cout << "\t\tUnable to create time values" << std::endl;
		exit(EXIT_FAILURE);
	};

	auto intermediatePath{makePath(OUT_DIRECTORY, CLEANED_FILE, OUTPUT_EXT)};
	if (writeCleanedFile(items, intermediatePath))
	{
		std::cout << "Writing:\t" << intermediatePath << std::endl;
	}
	else
	{
		std::cout << "Unable to write cleaned file to: " << intermediatePath << std::endl;
		exit(EXIT_FAILURE);
	}

	auto includePath{makePath(INCLUDE_DIRECTORY, INCLUDE_FILE, INCLUDE_EXT)};
	if (writeFile(makeInclude(items), includePath))
	{
		std::cout << "Writing:\t" << includePath << std::endl;
	}
	else
	{
		std::cout << "\t\tUnable to write include file to: " << intermediatePath << std::endl;
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
