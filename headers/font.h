/**
 * @file font.cpp
 * @author Nick (nickjt@duck.com)
 * @brief A helper class to support convertion of a standard Adobe (BDF) font into an include file
 * that can be compiled into an embedded C++ application. 
 * @version 0.1
 * @date 2023-01-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#ifndef FONT_H
#define FONT_H

#include <string>
#include <sstream>
#include "glyph.h"

class Font
{
public:

	static const std::string Definitions();

	Font();
	/// <summary>
	/// Decode the passed tokens from BDF format
	/// </summary>
	/// <param name="tokens"></param>
	void LoadFont(std::vector<std::string> tokens);

	/// <summary>
	/// A summary of the font file created
	/// </summary>
	/// <returns>The font summary as a std::string</returns>
	const std::string ToSummary() const;

	/// <summary>
	/// Returns the generated include file
	/// </summary>
	/// <returns>The include file as a std::string</returns>
	const std::string ToIncludeFile() const;

	/// <summary>
	/// Gets the short tag for the font (used as a filename)
	/// </summary>
	/// <returns>The tag as a std::string</returns>
	const std::string Tag() const;

	/// <summary>
	/// Sets the short tag for the font (used as a filename)
	/// </summary>
	/// <param name="tag">The string to set as the tag</param>
	void Tag(std::string tag);

	/// <summary>
	/// The number of Glyphs in the font file
	/// </summary>
	/// <returns>The number of Glyphs in the font file</returns>
	const int GlyphCount() const;

private:
	/// <summary> 
	/// /// Loads the glyphs in the BDF
	/// /// </summary>
	/// /// <param name="tokens"></param>
	void LoadGlyphs(std::vector<std::string> tokens);

	/// <summary>
	/// Set the extent values for the range of characters included in the 
	/// font file
	/// </summary>
	void Extents();

	/// <summary>
	/// Adds include file guard defs and include directives
	/// </summary>
	/// <returns>The string with the guards text</returns>
	std::string const Guards() const;

	/// <summary>
	/// Generates the header for the font definition and the
	/// definition that appears at the end of the include file
	/// </summary>
	/// <returns></returns>
	const std::string FontDefinitionBlock() const;

	/// <summary>
	/// Generates the header for the Widths file
	/// </summary>
	/// <returns></returns>
	const std::string FontDefinition() const;

	/// <summary>
	/// Generates the header for the Widths file
	/// </summary>
	/// <returns></returns>
	const std::string BitmapDef() const;

	std::string GlyphComment();

	/// <summary>
	/// Generate a string containing the width data for the font
	/// </summary>
	/// <returns></returns>
	const std::string GlyphSpecDef() const;

	/// <summary>
	/// Generate a string containint the Bitmap data for the font
	/// </summary>
	/// <returns></returns>
	std::string GenerateBitmaps();

	/// <summary>
	/// Round the passed value to the least largest whole byte
	/// </summary>
	/// <param name="value">The value to convert</param>
	/// <returns>The small whole byte value larger than value</returns>
	const unsigned int RoundToByte(unsigned int value) const;

	/// <summary>
	/// Generates the footer for both files
	/// </summary>
	/// <returns>The footer sting</returns>
	static std::string GlyphDeclaration();

	/// <summary>
	/// Generates the declaration for the overall font
	/// </summary>
	/// <returns>The font declaration string</returns>
	static std::string FontDeclaration();

	/// <summary>
	/// Summary string for the font file prviding the extents and other data
	/// </summary>
	/// <returns>A string showing the extents and other data</returns>
	const std::string FontSummary() const;

	/// <summary>
	/// Short name for the font. Also used
	/// for the font file root
	/// </summary>
	std::string tag_;

	/// <summary>
	/// The glyphs to include in the font file
	/// </summary>
	std::vector<Glyph> glyphs;


	std::string Name;
	int Width;
	int Height;
	int xOffset;
	int yOffset;
	int TotalGlyphs;

	int minEncoding_;
	int maxEncoding_;
	int vStep;

	int bbhMax;
	int bbhMin;
	int bbwMax;
	int bbwMin;
	int bbxMax;
	int bbxMin;
	int bbyMax;
	int bbyMin;
	int dWidthMax;
	int dWidthMin;
	int vExtentMax;

	int ParseInt(std::string s);

};


#endif