#include "frameBuffer.h"

FrameBuffer::FrameBuffer() : data{},
                             bufSpan{data},
                             origin{Point(0, 0)},
                             topRight{Point(MAX_X, 0)},
                             bottomRight{Point(MAX_X, MAX_Y_BITS)},
                             bottomLeft{Point(0, MAX_Y_BITS)}
{
    clear();
}

void FrameBuffer::set(Point const point)
{
    set(Pixel(point));
}

void FrameBuffer::clear()
{
    memset(data, 0, sizeof(data));
}

void FrameBuffer::line(Point const p1, Point const p2)
{
    Gradient g(p1, p2);
    set(p1);
    for (auto index{0}; index < g.steps(); index++)
    {
        set(g.next());
    }
}

void FrameBuffer::line(Point const p, Orientation orient, uint32_t length)
{
    if (orient == Orientation::horizontal)
    {
        line(p, Point(p.xVal() + length, p.yVal()));
    }
    else
    {
        line(p, Point(p.xVal(), p.yVal() + length));
    }
}

void FrameBuffer::set(std::vector<Point> points)
{
    for (auto &p : points)
    {
        set(p);
    }
}

void FrameBuffer::border()
{
    line(origin, topRight);
    line(topRight, bottomRight);
    line(bottomLeft, bottomRight);
    line(origin, bottomLeft);
}

void FrameBuffer::testPattern()
{
    line(bottomRight, origin);
    line(bottomLeft, topRight);
}

void FrameBuffer::set(Pixel pixel)
{
    data[pixel.index()] |= pixel.value();
}
