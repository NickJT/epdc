#pragma once
#define TESTING
#include <iostream>
#include <string>
#include <string_view>
#include "asset.h"
#include "picoDatetime.h"
#include "dimensions.h"

class QuoteServer
{

public:
	explicit QuoteServer(AssetStack assets);
	
	std::pair<bool, std::string_view> quoteFor(datetime_t dt);

	bool isDelimiter(char const c) const;
	bool isLineBreak(char const c) const;
	bool isEndOfText(char const c) const;
	std::string_view removeLeading(std::string_view raw);
#ifndef TESTING
private:
#endif
	AssetStack stack;
	bool isValidStack(AssetStack const asset) const;

	bool hasKey(const size_t key);
	int keyFrom(datetime_t dt);

	std::pair<bool, const Asset> GetAssetByKey(size_t key);
	std::pair<bool, std::string_view> const GetAssetText(const Asset asset);
	std::pair<bool, size_t> textCheck(size_t start);
	std::pair<bool, size_t> lengthCheck(size_t start);
};