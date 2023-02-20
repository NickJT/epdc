#include "quoteServer.h"
#include <cassert>

QuoteServer::QuoteServer(AssetStack assets) : stack{assets}
{
	if (!isValidStack(stack))
	{
		std::cout << "Stack is not valid" << std::endl;
		assert(isValidStack(stack));
	}
}

/**
 * @brief Gets a pointer to the first character in a quote for a given dt.
 * @param dt The datetime we want a quote for
 * @return std::pair<bool, const char *> first is true is a quote has been found. pointer is
 * first character or nullptr if first is false.
 */
std::pair<bool, std::string_view> QuoteServer::quoteFor(datetime_t dt)
{
	int key{keyFrom(dt)};
	if (hasKey(key))
	{
		if (auto [assetOk, asset]{GetAssetByKey(key)}; assetOk)
		{
			if (auto [textOk, sv]{GetAssetText(asset)}; textOk)
			{
				return {true, sv};
			}
		}
	}
	return {false, std::string_view{}};
}

std::string_view QuoteServer::removeLeading(std::string_view sv)
{
	auto pos{sv.find_first_not_of(WHITESPACE)};
	if (pos != std::string_view::npos)
	{
		sv.remove_prefix(pos);
	}
	return sv;
}

bool QuoteServer::isValidStack(AssetStack const stack) const
{
	auto expected{stack.MaxIndex};
	auto sz{stack.Text.size()};
	auto szb{stack.Text.size_bytes()};
	auto isTerminated{isEndOfText(stack.Text.back())};
	return sz == expected && szb == expected && isTerminated;
}

/// <summary>Check that the key is present in the asset stack</summary>
/// <param name= quoteStack>A reference to the quote stack</param>
/// <param name= key>The key for the quote we want to find</param>
/// <returns>true  If the key was found </returns>
/// <returns>false If the key wasn't found </returns>
bool QuoteServer::hasKey(const size_t key)
{
	for (auto const a : stack.Assets)
	{
		if (a.Key == key)
		{
			return true;
		}
	}
	return false;
}

int QuoteServer::keyFrom(datetime_t dt)
{
	return dt.hour * 60 + dt.min;
}

/// <summary>Get a memory pointer to the first char in the quote with this key.
/// <param name= quoteStack>A reference to the quote stack</param>
/// <param name= quoteNumber>The key of the quote required</param>
/// <returns>const char* A Flash memory pointer to the first char in the quote with the given key or nullptr if not found</returns>
std::pair<bool, std::string_view> const QuoteServer::GetAssetText(const Asset asset)
{
	if (auto [ok, length]{lengthCheck(asset.Index)}; ok)
	{
		std::string_view stg(&stack.Text[asset.Index]);
		return {true, stg};
	}
	return {false, std::string_view{}};
}

/**
 * @brief Checks that the pointer start is the first char of a valid string
 * of less than the maximum allowed length
 *  * @param start
 * @return bool True is length is less than the maximum defined in dimension.h
 */
std::pair<bool, size_t> QuoteServer::lengthCheck(size_t start)
{
    std::string_view quotesv(&stack.Text[start]);
 	return {quotesv.size()<= MAX_TEXT_LEN, quotesv.size()};
}

/// <summary>Gets a reference to a quote from a quotestack in memory using its key</summary>
/// <param name= stack>A reference to the quote stack in Flash memory</param>
/// <param name= key>The 16-bit key for the quote required</param>
/// <returns>Asset The reference to the required quote (in Flash) or nullptr otherwise</returns>
std::pair<bool, const Asset> QuoteServer::GetAssetByKey(size_t key)
{
	for (auto const asset : stack.Assets)
	{
		if (asset.Key == key)
			return {true, asset};
	}
	return {false, Asset{}};
}

bool QuoteServer::isDelimiter(char const c) const
{
	return (c == SPACE) || (c == EOT);
}

bool QuoteServer::isLineBreak(char const c) const
{
	return ((c == CR) || (c == LF));
}

bool QuoteServer::isEndOfText(char const c) const
{
	return (c == EOT);
}