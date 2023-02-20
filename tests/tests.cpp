#include "geometry.h"
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <limits>

using namespace std;

#pragma region Dimension
string testDimension()
{
    vector<pair<uint_t, uint_t>> xTests{{0, 0}, {1, 1}, {-1, 0}, {-0, 0}, {-2, 0}, {-INT16_MAX, 0}, {INT16_MAX, MAX_X}, {16, 16}, {17, 17}, {MAX_X, MAX_X}, {WIDTH, MAX_X}, {1280, MAX_X}};
    vector<pair<uint_t, uint_t>> yBitTests{{0, 0}, {-1, 0}, {-0, 0}, {-2, 0}, {-INT16_MAX, 0}, {INT16_MAX, MAX_Y_BITS}, {16, 16}, {17, 17}, {128, 128}, {MAX_Y_BITS, MAX_Y_BITS}, {HEIGHT, MAX_Y_BITS}};
    vector<pair<uint_t, uint_t>> yByteTests{{0, 0}, {1, 1}, {-1, 0}, {-0, 0}, {-2, 0}, {-INT16_MAX, 0}, {INT16_MAX, MAX_Y_BYTE}, {15, 15}, {17, 17}, {MAX_Y_BYTE, MAX_Y_BYTE}, {HEIGHT, MAX_Y_BYTE}};
    vector<pair<uint_t, uint_t>> indexTests{{0, 0}, {1, 1}, {-1, 0}, {-0, 0}, {-2, 0}, {-INT16_MAX, 0}, {INT16_MAX, MAX_INDEX}};

    bool xPass = true;
    for (auto &item : xTests)
    {
        xPass &= item.second == clampX(item.first);
    }
    bool bits = true;
    for (auto &item : xTests)
    {
        bits &= item.second == clampX(item.first);
    }
    bool bytes = true;
    for (auto &item : xTests)
    {
        bytes &= item.second == clampX(item.first);
    }
    bool index = true;
    for (auto &item : indexTests)
    {
        index &= item.second == clampIndex(item.first);
    }
    return (xPass && bits && bytes && index) ? "passed" : "failed";
}

#pragma endregion

#pragma region Point
string testPoint()
{

    typedef tuple<uint_t, uint_t, uint_t, uint_t> data;

    vector<data> constructors{
        make_tuple<>(0, 0, 0, 0),
        make_tuple<>(-1, -1, 0, 0),
        make_tuple<>(-2, -2, 0, 0),
        make_tuple<>(-INT16_MAX, -INT16_MAX, 0, 0),
        make_tuple<>(15, 15, 15, 15),
        make_tuple<>(16, 16, 16, 16),
        make_tuple<>(127, 127, 127, 127),
        make_tuple<>(128, 128, 128, 127),
        make_tuple<>(0, MAX_Y_BITS, 0, MAX_Y_BITS),
        make_tuple<>(0, HEIGHT, 0, MAX_Y_BITS),
        make_tuple<>(MAX_X, 0, MAX_X, 0),
        make_tuple<>(WIDTH, 0, MAX_X, 0),
        make_tuple<>(WIDTH, HEIGHT, MAX_X, MAX_Y_BITS),
        make_tuple<>(0, INT16_MAX, 0, MAX_Y_BITS),
        // INT32_MAX is cast to INT16_MAX by the compiler and then clamped to zero
        make_tuple<>(INT32_MAX, 0, 0, 0)};

    bool constructorsPass = true;
    Point p0;
    constructorsPass &= p0.xVal() == 0 && p0.yVal() == 0;
    if (!constructorsPass)
    {
        cout << "Failed: default constructor -> "
             << p0 << endl;
    }

    for (auto &item : constructors)
    {
        Point p(get<0>(item), get<1>(item));
        constructorsPass &= (p.xVal() == get<2>(item) && (p.yVal() == get<3>(item)));
        if (!constructorsPass)
        {
            cout << "Failed (" << get<0>(item) << "," << get<1>(item) << ") -> "
                 << "(" << p.xVal() << "," << p.yVal() << ")" << endl;
        }
    }

    return (constructorsPass) ? "passed" : "failed";
}

string testPointAdd()
{

    vector<pair<Point, Point>> adding{
        make_pair<Point, Point>(Point(0, 0), Point(0, 0)),
        make_pair<Point, Point>({15, 15}, {30, 30}),
        make_pair<Point, Point>({1, MAX_Y_BITS}, {2, MAX_Y_BITS}),
        make_pair<Point, Point>({0, HEIGHT / 2}, {0, MAX_Y_BITS}),
        make_pair<Point, Point>({WIDTH / 2, 0}, {MAX_X, 0}),
        make_pair<Point, Point>({WIDTH, HEIGHT}, {MAX_X, MAX_Y_BITS})};

    bool addingPass = true;
    Point p1(1, 2);
    Point p2(3, 4);
    Point p0;

    p0 += p1;
    p0 += p2;

    for (auto &item : adding)
    {
        Point p(item.first);
        p += item.first;

        addingPass &= (p == item.second);
        if (!addingPass)
        {
            cout << "Failed " << item.first << " -> " << p << endl;
        }
    }

    return (addingPass) ? "passed" : "failed";
}

string testPointSubtract()
{
    typedef tuple<Point, Point, Point> data;

    vector<data> subbing{
        make_tuple<Point, Point, Point>(Point(0, 0), Point(0, 0), Point(0, 0)),
        make_tuple<Point, Point, Point>({15, 15}, {14, 14}, {1, 1}),
        make_tuple<Point, Point, Point>({15, 15}, {14, 14}, {1, 1}),
        make_tuple<Point, Point, Point>({14, 14}, {15, 15}, {0, 0}),
        make_tuple<Point, Point, Point>({0, 0}, {MAX_X, MAX_Y_BITS}, {0, 0}),
        make_tuple<Point, Point, Point>({WIDTH, HEIGHT}, {MAX_X, MAX_Y_BITS}, {0, 0}),
        make_tuple<Point, Point, Point>({MAX_X, MAX_Y_BITS}, {WIDTH, HEIGHT}, {0, 0})};

    bool subPass = true;

    for (auto &item : subbing)
    {
        Point p(get<0>(item));
        Point p1(get<1>(item));
        p -= p1;

        subPass &= (p == get<2>(item));
        if (!subPass)
        {
            cout << "Failed " << get<0>(item) << " - " << get<1>(item) << " = " << p << endl;
        }
    }

    return (subPass) ? "passed" : "failed";
}

string pointAssign()
{
    typedef tuple<int, Point, Point, bool> data;
    std::vector<data> tests;
    tests.push_back({{1}, {Point()}, {Point(0, 0)}, {true}});
    tests.push_back({{2}, {Point(0, 1)}, {Point(0, 1)}, {true}});
    tests.push_back({{3}, {Point(1, 0)}, {Point(1, 0)}, {true}});
    tests.push_back({{4}, {Point(1, 0)}, {Point(0, 1)}, {false}});
    tests.push_back({{5}, {Point(WIDTH, 0)}, {Point(MAX_X, 0)}, {true}});
    tests.push_back({{6}, {Point(0, WIDTH)}, {Point(0, MAX_Y_BITS)}, {true}});
    tests.push_back({{7}, {Point(UINT16_MAX, UINT16_MAX)}, {Point(0, 0)}, {true}});

    bool pass{false};

    for (auto &item : tests)
    {
        auto test{get<0>(item)};
        auto p0{get<1>(item)};
        Point p1;
        p1 = p0;
        pass = (p1 == get<2>(item)) == get<3>(item);
        if (!pass)
        {
            cout << "Failed test " << test << " " << p0 << " = " << p1 << " is " << std::boolalpha << get<3>(item) << endl;
            return "failed";
        }
    }

    return "passed";
}

#pragma endregion

#pragma region Pixel
string testPixel()
{
    bool constructorsPass = true;
    Pixel p0;
    constructorsPass &= p0.index() == 0 && p0.value() == 0;
    if (!constructorsPass)
    {
        cout << "Failed: default constructor -> "
             << p0 << endl;
    }

    vector<std::tuple<int, int, int, int, int>> constructors{
        make_tuple<>(1, 0, 0, 0, 0),
        make_tuple<>(2, -1, -1, 0, 255),
        make_tuple<>(3, -2, -2, 0, 254),
        make_tuple<>(4, -INT16_MAX, 255, 0, 255),
        make_tuple<>(5, 0, 1, 0, 1),
        make_tuple<>(6, 0, 255, 0, 255),
        make_tuple<>(7, MAX_INDEX, 0, MAX_INDEX, 0),
        make_tuple<>(8, HEIGHT * WIDTH / 8, 0, MAX_INDEX, 0),
        make_tuple<>(9, UINT16_MAX, 0, 0, 0),
        make_tuple<>(10, INT32_MAX, 0, 0, 0)};

    for (auto &item : constructors)
    {
        Pixel p(get<1>(item), get<2>(item));
        constructorsPass &= (p.index() == get<3>(item) && (p.value() == get<4>(item)));
        if (!constructorsPass)
        {
            cout << "Failed test " << get<0>(item) << "  Pixel(" << get<1>(item) << "," << get<2>(item) << ") -> " << p << endl;
            return "failed";
        }
    }
    return "passed";
}

std::string pixelFromPoint()
{
    typedef tuple<uint_t, Point, uint_t, uint8_t> data;

    bool constructorsPass = true;

    vector<data> constructors{
        make_tuple<>(1, Point(), 0, 0x80), // MSB of the first byte
        make_tuple<>(2, Point(0, 1), 0, 0x40),
        make_tuple<>(3, Point(0, 2), 0, 0x20),
        make_tuple<>(4, Point(0, 3), 0, 0x10),
        make_tuple<>(5, Point(0, 4), 0, 0x08),
        make_tuple<>(6, Point(0, 5), 0, 0x04),
        make_tuple<>(7, Point(0, 6), 0, 0x02),
        make_tuple<>(8, Point(0, 7), 0, 0x01),
        make_tuple<>(9, Point(0, 8), 1, 0x80),
        make_tuple<>(10, Point(0, 9), 1, 0x40),
        make_tuple<>(11, Point(1, 0), 16, 0x80),
        make_tuple<>(12, Point(1, 1), 16, 0x40),
        make_tuple<>(13, Point(MAX_X, MAX_Y_BITS), MAX_INDEX, 0x01),
    };

    for (auto &item : constructors)
    {
        auto point{get<1>(item)};
        Pixel pix{Pixel(point)};
        constructorsPass &= (pix.index() == get<2>(item) && (pix.value() == get<3>(item)));
        if (!constructorsPass)
        {
            cout << "Failed test " << get<0>(item) << "  Point" << get<1>(item) << " -> " << pix << endl;
            return "failed";
        }
    }
    return "passed";
}

string pixelArith()
{
    typedef tuple<uint_t, Pixel, Pixel, bool> data;

    vector<data> tests{
        make_tuple<>(1, Pixel(), Pixel(), true),
        make_tuple<>(2, Pixel(0, 255), Pixel(0, 0), false),
        make_tuple<>(3, Pixel(0, 255), Pixel(0, 255), true),
        make_tuple<>(4, Pixel(0, 253), Pixel(0, 255), false),
        make_tuple<>(5, Pixel(1, 0), Pixel(1, 0), true),
        make_tuple<>(6, Pixel(1, 255), Pixel(1, 0), false),
        make_tuple<>(7, Pixel(0, 6), Pixel(6, 0), false),
        make_tuple<>(8, Pixel(255, 0), Pixel(0, 255), false),
        make_tuple<>(9, Pixel(MAX_INDEX, 0), Pixel(MAX_INDEX, 0), true),
        make_tuple<>(10, Pixel(MAX_INDEX, 0), Pixel(MAX_INDEX, 1), false),
        make_tuple<>(11, Pixel(MAX_INDEX, 255), Pixel(MAX_INDEX, 255), true),
        make_tuple<>(12, Pixel(MAX_INDEX, 255), Pixel(MAX_INDEX, 254), false),
    };

    bool pass{false};
    for (auto &item : tests)
    {
        auto p1{get<1>(item)};
        auto p2{get<2>(item)};
        Pixel p3 = p2;
        bool result{get<3>(item)};
        pass = ((p1 == p2) == result);

        if (!pass)
        {
            cout << "Failed test " << get<0>(item) << " " << p1 << " == " << p2 << " is " << get<3>(item) << endl;
            return "failed";
        }

        pass = ((p1 == p3) == result);
        if (!pass)
        {
            cout << "Failed test "
                 << " " << get<0>(item) << p1 << " = " << p3 << " is " << get<3>(item) << endl;
            return "failed";
        }
    }

    return "passed";
}

string gradient2()
{
    typedef tuple<int, Point, Point> data;
    vector<data> tests;
    tests.push_back({{1}, {0, 0}, {0, 2}});
    tests.push_back({{2}, {0, 0}, {2, 0}});
    tests.push_back({{3}, {0, 2}, {0, 0}});
    tests.push_back({{4}, {2, 0}, {0, 0}});
    tests.push_back({{5}, {7, 10}, {3, 3}});
    tests.push_back({{6}, {10, 7}, {3, 3}});
    tests.push_back({{7}, {0, 0}, {MAX_X, MAX_Y_BITS}});
    tests.push_back({{8}, {0, 0}, {MAX_X, 0}});
    tests.push_back({{9}, {0, 0}, {0, MAX_Y_BITS}});
    tests.push_back({{10}, {MAX_X, MAX_Y_BITS}, {MAX_X - 2, 0}});
    tests.push_back({{11}, {20, 120}, {3, 200}});
    tests.push_back({{12}, {42, 119}, {198, 110}});
    tests.push_back({{13}, {3, 0}, {4, 127}});
    tests.push_back({{14}, {294, 1}, {50, 50}});

    for (auto &test : tests)
    {
        auto a{get<1>(test)};
        auto b{get<2>(test)};
        auto g{Gradient(a, b)};
        Point nxt;
        auto numberOfSteps{g.steps()};
        for (int s{0}; s < numberOfSteps; s++)
        {
            nxt = g.next();
        }

        bool pass{nxt == b};

        if (!pass)
        {
            pass = (nxt.xVal() == (b.xVal() - 1)) && (nxt.yVal() == b.yVal());
        }

        if (!pass)
        {
            pass = (nxt.xVal() == b.xVal()) && ((nxt.yVal()) == b.yVal() - 1);
        }

        if (!pass)
        {
            cout << "\n\rFailed test " << get<0>(test) << " " << a << " to "
                 << b << ": Finished at" << nxt << endl;
            return "failed";
        }
    }

    return "passed";
}
#pragma endregion

int main()
{
    cout << "Dimension Test:\t" << testDimension() << endl;
    cout << "Point Test 1:\t" << testPoint() << endl;
    cout << "Point Test 2:\t" << pointAssign() << endl;
    cout << "Point Test 3:\t" << testPointAdd() << endl;
    cout << "Point Test 4:\t" << testPointSubtract() << endl;
    cout << "Pixel Test 1:\t" << testPixel() << endl;
    cout << "Pixel Test 2:\t" << pixelFromPoint() << endl;
    cout << "Pixel Test 3:\t" << pixelArith() << endl;
    cout << "Pixel Test 4:\t" << testPixel() << endl;
    cout << "Pixel Test 5:\t" << pixelFromPoint() << endl;
    cout << "Pixel Test 6:\t" << pixelArith() << endl;
    cout << "Gradient Test:\t" << gradient2() << endl;
}