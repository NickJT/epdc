#include <iostream>
#include <cassert>
#include <iomanip>
#include <span>
#include <string_view>

#include "dimensions.h"
#include "quoteServer.h"
#include "timeQuotes.h"
#include "picoDatetime.h"

using namespace std;

AssetStack stack{timeText, timeAssets, 776, 76030};

std::ostream &operator<<(std::ostream &os, datetime_t dt)
{
    os << std::setw(2) << std::setfill('0') << (int)dt.hour
       << ":" << std::setw(2) << std::setfill('0') << (int)dt.min
       << ":" << std::setw(2) << std::setfill('0') << (int)dt.sec << " ";
    return os;
}

void quoteTest(QuoteServer &qs, string_view expected, datetime_t dt)
{
    cout << "Quote for: " << dt << "Key: ";
    cout << qs.keyFrom(dt) << "  Index: " << qs.GetAssetByKey(qs.keyFrom(dt)).second.Index << " - ";
    auto [ok0, result]{qs.quoteFor(dt)};
    assert(ok0);
    assert(result == expected);
    cout << "passed\n\r";
}

void lengthTest(QuoteServer &qs, string_view expected, datetime_t dt)
{
    cout << "Length test for: " << dt;
    auto index{qs.GetAssetByKey(qs.keyFrom(dt)).second.Index};
    char const *quoteStartAddress{&stack.Text[index]};
    string_view quotesv(quoteStartAddress);
    assert(expected.size() == quotesv.size());
    cout << " passed\n\r";
}

int main()
{
    cout << "Stack validity - ";
    QuoteServer qs(stack);
    cout << "passed\n\r";

    cout << "isDelimiter - ";
    assert(qs.isDelimiter(SPACE));
    assert(qs.isDelimiter(0));
    assert(!qs.isDelimiter('\n'));
    assert(!qs.isDelimiter('\r'));
    assert(!qs.isDelimiter('\r'));
    assert(!qs.isDelimiter('a'));
    cout << " passed \n\r";

    cout << "isLineBreak - ";
    assert(qs.isLineBreak(LF));
    assert(qs.isLineBreak(CR));
    assert(!qs.isLineBreak(EOT));
    assert(!qs.isLineBreak(SPACE));
    assert(!qs.isLineBreak('a'));
    cout << "passed \n\r";

    cout << "isEndOfText - ";
    assert(qs.isEndOfText(EOT));
    assert(!qs.isEndOfText(CR));
    assert(!qs.isEndOfText(LF));
    assert(!qs.isEndOfText(SPACE));
    assert(!qs.isEndOfText('a'));
    cout << "passed\n\r";

    cout << "removeLeading - ";
    string_view noWhiteSpace{"alpha"};
    string_view hasSpace{" alpha"};
    string_view hasSpaces{"          alpha"};
    string_view hasLF{"\nalpha"};
    string_view hasCR{"\ralpha"};
    string_view hasBoth{"\n\ralpha"};
    string_view hasBackSpaces{"alpha   "};

    assert(qs.removeLeading(noWhiteSpace) == noWhiteSpace);
    assert(qs.removeLeading(hasSpace) == noWhiteSpace);
    assert(qs.removeLeading(hasSpaces) == noWhiteSpace);
    assert(qs.removeLeading(hasLF) == noWhiteSpace);
    assert(qs.removeLeading(hasCR) == noWhiteSpace);
    assert(qs.removeLeading(hasBoth) == noWhiteSpace);
    assert(qs.removeLeading(hasBackSpaces) == hasBackSpaces);
    cout << "passed\n\r";

    string_view quote0000{"Big Ben had once again struck midnight. The time outside still corresponded to that registered by the stopped gilt clock, inside."};
    string_view quote0007{"It was seven minutes after midnight. The dog was lying on the grass in the middle of the lawn."};
    string_view quote0317{"192 different still lives of Green Oaks at 3.17 a.m. this March night"};
    string_view quote2325{"They were plainly visible, and the hands indicated the hour of eleven o'clock and twenty-five minutes."};
    string_view quote2357{"Chigurh raised his wrist and looked at his watch. Eleven fifty-seven he said. Wells nodded."};

    datetime_t dt{
        .hour = 0,
        .min = 0,
        .sec = 0,
    };

    quoteTest(qs, quote0000, dt);
    lengthTest(qs, quote0000, dt);
    dt.min = 7;
    quoteTest(qs, quote0007, dt);
    lengthTest(qs, quote0007, dt);
    dt.hour = 03;
    dt.min = 17;
    quoteTest(qs, quote0317, dt);
    lengthTest(qs, quote0317, dt);
    dt.hour = 23;
    dt.min = 25;
    quoteTest(qs, quote2325, dt);
    lengthTest(qs, quote2325, dt);
    dt.hour = 23;
    dt.min = 57;
    quoteTest(qs, quote2357, dt);
    lengthTest(qs, quote2357, dt);

    return 0;
}