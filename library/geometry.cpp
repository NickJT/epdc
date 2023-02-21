
#include "geometry.h"

std::ostream &operator<<(std::ostream &os, const Point &point)
{
    os << '(' << point.xVal() << ',' << point.yVal() << ')';
    return os;
}

std::ostream &operator<<(std::ostream &os, const Pixel &pixel)
{
    os << '[' << pixel.index() << "]: 0x" << std::hex << std::uppercase << static_cast<uint_t>(pixel.value());
    return os;
}

std::ostream &operator<<(std::ostream &os, Gradient &g)
{
    os << " steps = " << g.steps();
    return os;
}

std::ostream &operator<<(std::ostream &os, const Orientation &orn)
{
    switch (orn)
    {
    case Orientation::horizontal:
        os << "horizontal";
        break;
    case Orientation::vertical:
        os << "vertical";
        break;
    default:
        os << "unknown";
        break;
    }
    return os;
}

#pragma region Point

Point::Point() : x{0}, y{0}
{
}

Point::Point(uint_t const x_, uint_t const y_) : x{clampX(x_)}, y{clampYBits(y_)} {}

Point &Point::operator-=(const Point &a)
{
    x = (x - a.x) > 0 ? x - a.x : 0;
    y = (y - a.y) > 0 ? y - a.y : 0;
    return *this;
}

Point &Point::operator+=(const Point &a)
{
    x = clampX(x + a.x);
    y = clampYBits(y + a.y);
    return *this;
}

Point &Point::operator=(const Point &rhs)
{
    x = clampX(rhs.x);
    y = clampYBits(rhs.y);
    return *this;
}

bool Point::operator==(const Point &rhs)
{
    return ((x == rhs.xVal()) && (y == rhs.yVal()));
}
#pragma end region

#pragma region Pixel

Pixel::Pixel() : idx{0}, val{0}
{
}

Pixel::Pixel(uint_t const index_, uint8_t const value_)
{
    idx = clampIndex(index_);
    val = value_;
}

Pixel::Pixel(Point p)
{
    // It shouldn't be possible to create an invalid Point so
    // we don't really need this clamping
    auto x{clampX(p.xVal())};
    auto y{clampYBits(p.yVal())};
    if (x != p.xVal() || y != p.yVal())
    {
        //std::cout << "\n\rReceived an invalid Point" << std::endl;
    }
    /* Note this only works for Height = 16 and probably isn't faster after optimisation 
    idx = (y << 4U) + (x >> 3U);         // mod
    val = (0b1U << (7U - (y & 0b111U))); // rem
    */
    div_t res{div(y, 8)};
    idx = (x * (HEIGHT / 8)) + res.quot;
    val = (0b1 << (7 - res.rem));
}

bool Pixel::operator==(const Pixel &rhs)
{
    return ((idx == rhs.index()) && (val == rhs.value()));
}

Pixel &Pixel::operator=(const Pixel &rhs)
{
    idx = rhs.index();
    val = rhs.value();
    return *this;
}

#pragma end region

Gradient::Gradient() : stepX{0}, stepY{0}, x{0}, y{0}
{
    numberOfSteps = 0;
}

Gradient::Gradient(Point a, Point b) : stepX{0}, stepY{0}
{
    auto ax{a.xVal()};
    auto ay{a.yVal()};
    auto bx{b.xVal()};
    auto by{b.yVal()};
    deltaX = bx - ax;
    deltaY = by - ay;
    numberOfSteps = std::max(abs(deltaX), abs(deltaY));
    if (numberOfSteps > 0)
    {
        stepX = (deltaX << shift) / numberOfSteps;
        stepY = (deltaY << shift) / numberOfSteps;
    }

    x = ax << shift;
    y = ay << shift;
}

int Gradient::steps() { return numberOfSteps; }
Point Gradient::next()
{
    x += stepX;
    y += stepY;
    return Point(x >> shift, y >> shift);
}

