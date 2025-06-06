
#include "layoutServer.h"
#include "styleSheets.h"
#include "timeQuotes.h"
#include "displayDriver.h"
#include "debug.h"

LayoutServer::LayoutServer(std::unique_ptr<displayDriver> hardwareDriver) : qs{QuoteServer(AssetStack(timeText, timeAssets, 3108, 671829))},
                                                                            fs{},
                                                                            driver{std::move(hardwareDriver)}
{
    dbg("Layout server instantiated" << std::endl);
}

void LayoutServer::timeIs(datetime_t t)
{
    dbg("Time is " << (int)t.hour << ":" << (int)t.min << std::endl);
    driver->clear();
    auto [quoteFound, quote]{qs.quoteFor(t)};
    if (quoteFound)
    {
        layoutQuote(quote);
    }
    else
    {
        layoutClockFace(TopCat::toClockTime(t));
    }
    driver->update();
}

void LayoutServer::cmd(Button const b)
{
    dbg("\n\rGot button " << static_cast<int>(b) << std::endl);
}

void LayoutServer::layoutQuote(std::string_view q)
{
    int rowMargin = quoteStyle.rowMargin;

    fs = FontServer(quoteStyle.font);
    auto verts{fs.fontVerticals()};
    int vStep{rowMargin + verts.verticalStep};

    // home isn't used yet but we may add border gap etc from style sheet later
    int homeY = quoteStyle.homeY;
    int originY = homeY + verts.maxRise;

    int homeX = quoteStyle.homeX;
    int originX = homeX + quoteStyle.originX;

    int maxLine = WIDTH;
    // End of page setup

    std::vector<std::string_view> lines{wordWrap(q, maxLine)};
    for (auto const &line : lines)
    {
        renderLine(line, originX, originY);
        originY = originY + vStep;
    }
}

void LayoutServer::layoutClockFace(std::string timeString)
{
    fs = FontServer(clockStyle.font);
    auto verts{fs.fontVerticals()};
    int originY = clockStyle.homeY + clockStyle.originY + verts.maxRise;

    int originX = clockStyle.homeX + clockStyle.originX;
    renderLine(timeString, originX, originY);
}

size_t LayoutServer::setFirstCharOrigin(size_t home, char c)
{
    int firstCharOffset{fs.glyphFor(c).bbx};
    if (firstCharOffset < 0)
    {
        return home - firstCharOffset; // e.g. if offset is -2 return home + 2
    }
    return home;
}

void LayoutServer::renderLine(std::string_view line, int originX, int originY)
{
    originX = setFirstCharOrigin(originX, line.at(0));
    int startX{0};
    int startY{0};
    for (auto const c : line)
    {
        auto glyph{fs.glyphFor(c)};
        startX = originX + glyph.bbx;
        startY = originY - (glyph.bbh + glyph.bby);
        renderChar(glyph, fs.bitsFor(c), startX, startY);
        originX = originX + glyph.DWidth;
    }
}

void LayoutServer::renderChar(BdfGlyph g, std::vector<bool> v, uint_t x, uint_t y)
{
    size_t widthInBytes = (g.bbw + 7) >> 3;
    size_t widthInBits{widthInBytes * 8};
    size_t totalBits{widthInBits * g.bbh};
    if (totalBits != v.size())
    {
        return;
    }
    uint_t col{x};
    uint_t row{y};
    for (auto b : v)
    {
        if (b)
        {
            driver->set(col, row);
        }
        col++;
        if (col == x + widthInBits)
        {
            col = x;
            row++;
        }
    }
}

size_t LayoutServer::skipWhitespace(std::string_view raw, size_t start)
{
    auto pos{raw.find_first_not_of(WHITESPACE, start)};
    return (pos == std::string_view::npos) ? raw.size() : pos;
}

std::vector<std::string_view> LayoutServer::wordWrap(std::string_view s, size_t len)
{
    std::vector<std::string_view> result;
    size_t lineStart{skipWhitespace(s, 0)};
    size_t lineEnd{wrapper(s, lineStart, len)};
    while (lineStart < s.size() && (lineStart != lineEnd))
    {
        result.push_back(s.substr(lineStart, lineEnd - lineStart));
        lineStart = skipWhitespace(s, lineEnd);
        lineEnd = wrapper(s, lineStart, len);
    }
    return result;
}

size_t LayoutServer::wrapper(std::string_view s, size_t pos, size_t len)
{
    size_t last{pos};
    size_t w{0};

    while (pos < s.size())
    {
        if (w > len)
        {
            return last;
        }
        if (qs.isLineBreak(s.at(pos)))
        {
            return pos;
        }
        if (qs.isDelimiter(s.at(pos)))
        {
            last = pos;
        }
        w += fs.widthOf(s.at(pos));
        pos++;
    }
    return (w > len) ? last : s.size();
}
