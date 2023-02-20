#pragma once

#include <cinttypes>
#include <span>

/// <summary>Declaration for the Asset struct used to retrieve and format the asset text</summary>
struct Asset
{
	///Space for a 16-bit key (currently holds the zero-based ordinal number of the asset)
	size_t Key;
	///Byte index into the asset array for this asset
	size_t Index;
	///A code for the required font (not used yet)
	uint8_t FontId;
	///A code for the required style (not used yet)
	uint8_t Style;
};

/// <summary>Declaration for the struct describing the overall stack of assets</summary>
struct AssetStack
{
	///The const char array containing the asset text. Cant be a string_view because we have 
	/// multiple EOT characters
	std::span<const char> Text;
	///The Asset struct in the array containing the asset details
	std::span<const Asset> Assets;
	///The total number of assets in this stack
	const uint16_t Quantity;
	///The maximum index in the text table (i.e. the compiled length in bytes of the asset text)
	const uint32_t MaxIndex;
};


