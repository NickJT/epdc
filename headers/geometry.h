#pragma once

#include "dimensions.h"
#include <iostream>

/**
 * @brief Clamps the passed value to the maximum permited horizontal (x) value
 *
 * @param x - the value to clamp
 * @return the minimum of x and MAX_X
 */
inline static uint_t clampX(uint_t const x)
{
    if (x & signBit)
        return 0;
    else
        return std::min(x, MAX_X);
}

/**
 * @brief Clamps the passed value to the maximum permited vertical (y) value
 *
 * @param y - the value to clamp
 * @return the minimum of y and MAX_Y_BITS
 */
inline static uint_t clampYBits(uint_t const y)
{
    if (y & signBit)
        return 0;
    else
        return std::min(y, MAX_Y_BITS);
}

/**
 * @brief Clamps the passed value to the maximum permited vertical (y) byte value
 *
 * @param y - the value to clamp
 * @return the minimum of y and MAX_Y_BYTE
 */
inline static uint_t clampYByte(uint_t const y)
{
    if (y & signBit)
        return 0;
    else
        return std::min(y, MAX_Y_BYTE);
}

/**
 * @brief Clamps the passed value to the maximum permited vertical (y) byte value
 *
 * @param y - the value to clamp
 * @return the minimum of y and MAX_Y_BYTE
 */
inline static uint_t clampIndex(uint_t const i)
{
    if (i & signBit)
        return 0;
    else
        return std::min(i, MAX_INDEX);
}

enum class Orientation
{
    horizontal,
    vertical,
};

class Point
{
public:
    Point();
    Point(uint_t const x_, uint_t const y_);

    inline const uint_t xVal() const { return x; }
    inline const uint_t yVal() const { return y; }

    Point &operator-=(const Point &rhs);
    Point &operator+=(const Point &rhs);
    Point &operator=(const Point &rhs);
    bool operator==(const Point &rhs);

private:
    uint_t x;
    uint_t y;
};

std::ostream &operator<<(std::ostream &os, const Point &point);

/**
 * @brief Convenience class to hold the value of a pixel as an index into the
 * frame buffer and a byte value. Or'ing a pixel into the buffer will set the
 * pixel to black.
 */
class Pixel
{
public:
    /**
     * @brief Construct a new Pixel object with index and value of zero. The
     * passed index value is clamped to MAX_INDEX
     */
    Pixel();

    /**
     * @brief Construct a new Pixel object
     *
     * @param idx_ an index into the frame buffer (clampled to MAX_INDEX)
     * @param value_  the value needed to set a single pixel
     */
    Pixel(uint_t const idx_, uint8_t const value_);

    /**
     * @brief Construct a new Pixel object from a point (x,y). The
     * index is set from the x and y coordinates and the value is set to zero
     * (i.e. no pixels set)
     */
    Pixel(Point);

    inline uint_t const index() const { return idx; }
    inline uint8_t const value() const { return val; }

    bool operator==(const Pixel &rhs);
    Pixel &operator=(const Pixel &rhs);

private:
    uint_t idx;
    uint8_t val;
};

std::ostream &operator<<(std::ostream &os, const Pixel &pixel);

constexpr uint8_t shift{16};

class Gradient
{
public:
    Gradient();
    Gradient(Point a, Point b);
    int steps();
    Point next();

private:
    int stepX;
    int stepY;
    int x;
    int y;
    int deltaX;
    int deltaY;
    
    uint_t numberOfSteps;
};

std::ostream &operator<<(std::ostream &os, Gradient &s);

class Rect
{

public:
    Rect();

private:
    Point tl;
    Point br;
};