
#include <string>
#include <cctype>
#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <locale>
#include <codecvt>
#include "font.h"
#include "glyph.h"

/*
The Adobe BDF format is here: https://www.adobe.com/content/dam/acom/en/devnet/font/pdfs/5005.BDF_Spec.pdf
BDF files in this format can be generated or edited with: https://fontforge.org/en-US/
*/

/// <summary>
/// The include file containing the definitions for the glyph and font classes are written to
/// 'DefinitionFileStem'.h
/// </summary>
const std::string DefinitionFileStem{"FontDefs"};

/// <summary>
/// The BDF file tokenization loop terminates before encoding GlyphCountLimit is reached (usually asciiTilda)
/// </summary>
constexpr int GlyphCountLimit{127};
// constexpr int GlyphCountLimit{58};

void PrintArguments()
{
    std::cout << "Optional Usage: "
              << "FontGenerator"
              << "  [SOURCE]  [DESTINATION]  [-r(ecursive)]" << std::endl;
}

void ParseArguments(int argc, char *argv[], std::string &inStg, std::string &outStg, bool &recursive)
{
    if (argc > 2)
    {
        inStg = std::string(argv[1]);
        outStg = std::string(argv[2]);
        if (argc == 4)
        {
            recursive = ((argv[3][0] == '-') && (argv[3][1] == 'r'));
        }
    }
    else
    {
        PrintArguments();
    }
}

std::string DefaultDirArg()
{
    std::string defArg{"."};
    defArg += std::filesystem::path::preferred_separator;
    return defArg;
}

void TestPath(std::filesystem::path inPath)
{
    using namespace std;
    cout << "Root directory: " << inPath.root_directory() << endl;
    cout << "Root name: " << inPath.root_name() << endl;
    cout << "Root path: " << inPath.root_path() << endl;
    cout << "Relative path: " << inPath.relative_path() << endl;
    cout << "Parent path: " << inPath.parent_path() << endl;
    cout << "Filename: " << inPath.filename() << endl;
    cout << "Stem: " << inPath.stem() << endl;
    cout << "Extension: " << inPath.extension() << endl;

    if (!std::filesystem::is_directory(inPath))
    {
        return;
    }

    std::filesystem::recursive_directory_iterator dirIt;
    cout << "Iterating over directory" << endl;
    for (const std::filesystem::directory_entry &f : std::filesystem::directory_iterator(inPath))
    {
        if (f.is_regular_file())
        {
            cout << "Filename: " << inPath.filename() << "\t";
            cout << "Stem: " << inPath.stem() << "\t";
            cout << "Extension: " << inPath.extension() << endl;
        }
    }
}

bool isBdf(const std::filesystem::directory_entry &f)
{
    if (f.is_regular_file())
    {
        auto filePath{f.path()};
        if (filePath.has_extension())
        {
            if (filePath.extension() == ".bdf")
            {
                return true;
            }
        }
    }
    return false;
}

std::vector<std::filesystem::path> GetSourceFilesNonRecursive(std::string iString)
{
    std::vector<std::filesystem::path> result;
    std::filesystem::path inPath{iString};

    std::filesystem::directory_iterator dI(inPath);

    for (const std::filesystem::directory_entry &f : dI)
    {
        if (isBdf(f))
        {
            result.push_back(f);
        }
    }
    return result;
}

std::vector<std::filesystem::path> GetSourceFilesRecursive(std::string iString)
{
    std::vector<std::filesystem::path> result;
    std::filesystem::path inPath{iString};

    std::filesystem::recursive_directory_iterator rdI(inPath);

    for (const std::filesystem::directory_entry &f : rdI)
    {
        if (isBdf(f))
        {
            result.push_back(f);
        }
    }
    return result;
}

std::vector<std::filesystem::path> GetSourceFiles(std::string iString, bool recursive)
{
    if (recursive)
    {
        return GetSourceFilesRecursive(iString);
    }
    else
    {
        return GetSourceFilesNonRecursive(iString);
    }
}

void ListFiles(std::vector<std::filesystem::path> filepaths)
{
    if (filepaths.size() > 0)
    {
        std::cout << filepaths.size() << " BDF files to be converted" << std::endl;

        for (const auto &f : filepaths)
        {
            std::cout << f.filename() << std::endl;
        }
    }
    else
    {
        std::cout << "No BDF files found" << std::endl;
    }
}

int GetValue(std::string stg)
{
    int value{-1};
    try
    {
        value = std::stoi(stg);
    }
    catch (std::invalid_argument const &e)
    {
        value = -1;
    }
    catch (std::out_of_range const &e)
    {
        value = -1;
    }
    return value;
}

int CheckEncoding(const std::string label, const std::string value)
{
    if (!label.starts_with("ENCODING"))
    {
        return -1;
    }
    return GetValue(value);
}

std::string GetFileContent(std::filesystem::path fPath)
{
    std::ifstream bdfFile(fPath.make_preferred());
    std::stringstream ss;

    if (bdfFile.is_open())
    {
        ss << bdfFile.rdbuf();
        bdfFile.close();
    }
    return ss.str();
}

/// <summary>Tokenizes the passed file to extract up to 'maxGlyphs' glyph descriptors</summary>
/// <param name="fPath">The path to the bdf file</param>
/// <returns>A vector of wide string tokens</returns>
std::vector<std::string> TokenizeBdf(const std::string &content, int maxGlyphs)
{
    std::vector<std::string> tokens;
    std::string lastToken{};

    std::string delimiters{"\n "};
    int encoding{-1};

    std::string::size_type wStart;
    wStart = content.find_first_not_of(delimiters);

    while ((wStart != std::string::npos) && (encoding < maxGlyphs))
    {
        std::string::size_type wEnd;
        wEnd = content.find_first_of(delimiters, wStart);
        if (wEnd == std::string::npos)
        {
            wEnd = content.length();
        }
        std::string token{content.substr(wStart, wEnd - wStart)};
        wStart = content.find_first_not_of(delimiters, wEnd);

        tokens.push_back(token);
        encoding = CheckEncoding(lastToken, token);
        lastToken = token;
    }
    return tokens;
}

Font MakeFont(std::vector<std::string> &&tokens, std::string tag)
{
    Font font;
    font.LoadFont(tokens);
    font.Tag(tag);
    return font;
}

std::filesystem::path MakeFontFilePath(const std::string outDir, const std::string stem)
{
    std::filesystem::path outPath{outDir};
    outPath.make_preferred();
    outPath += std::filesystem::path::preferred_separator;
    outPath += stem + ".h";
    return outPath;
}

bool WriteFont(const Font &&font, std::filesystem::path p)
{
    if (font.GlyphCount() == 0)
    {
        return false;
    }

    std::ofstream oFile(p);
    if (!oFile)
    {
        return false;
    }

    oFile << font.ToIncludeFile() << std::endl;
    oFile.close();

    return true;
}

bool WriteDefinitionFile(std::string dirStg)
{
    std::ofstream oFile(MakeFontFilePath(dirStg, DefinitionFileStem));
    if (!oFile)
    {
        return false;
    }

    oFile << Font::Definitions();
    oFile.close();
    return true;
}

int main(int argc, char *argv[])
{
    using namespace std;

    bool recursive{false};
    std::string inString{DefaultDirArg()};
    std::string outString{inString};

    ParseArguments(argc, argv, inString, outString, recursive);

    std::vector<std::filesystem::path> sourceFilePaths{GetSourceFiles(inString, recursive)};
    ListFiles(sourceFilePaths);

    for (auto &p : sourceFilePaths)
    {
        std::vector<std::string> bdfTokens{TokenizeBdf(GetFileContent(p), GlyphCountLimit)};
        auto font{MakeFont(std::move(bdfTokens), p.stem().string())};

        std::filesystem::path fontFilePath{MakeFontFilePath(outString, font.Tag())};
        std::cout << "Writing " << font.Tag() << " to " << fontFilePath << std::endl;

        if (WriteFont(std::move(font), fontFilePath))
        {
            std::wcout << "Font written to " << fontFilePath << std::endl;
        }
    }
    /*
        if (WriteDefinitionFile(outString))
        {
            std::cout << "Font definition include file written to " << MakeFontFilePath(outString, DefinitionFileStem) << "\n\r" << std::endl;
        };
    */
    std::cout << "Done" << std::endl;
}
