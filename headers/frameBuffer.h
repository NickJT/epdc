#pragma once

#include <cinttypes>
#include <cstring>
#include <memory>
#include <vector>

#include "dimensions.h"
#include "geometry.h"
#include "displayDriver.h"

class FrameBuffer
{
public:
    explicit FrameBuffer();
    void set(Point const point);
    void set(std::vector<Point> points);
    void clear();
    void line(Point const p1, Point const p2);
    void line(Point const p, Orientation orient, uint32_t length);
    void border();
    inline uint_t width() { return WIDTH; }
    inline uint_t height() { return HEIGHT; }
    uint8_t data[FRAMEBUFFERSIZE];

private:
    std::span<uint8_t> bufSpan;
    const Point origin;
    const Point topRight;
    const Point bottomRight;
    const Point bottomLeft;
    void set(Pixel p);
    void testPattern();
};
