#pragma once

#include <iostream>
#include <cstring>
#include <string_view>
#include <chrono>
#include <thread>
#include <future>
#include <cassert>

#include <X11/Xlib.h>
 #include <X11/Xutil.h>
#include "displayDriver.h"
#include "dimensions.h"
#include "geometry.h"

/* X11 "Some functions return Status, an integer error indication. If the function fails, it returns a zero.
The X server reports protocol errors at the time that it detects them. When Xlib detects an error, it calls
an error handler, which your program can provide. If you do not provide an error handler,
the error is printed, and your program terminates."
*/

/**
 * @brief Currently we use the default screen for the display but this could
 *  also be hardcoded so we don't have to rely on the DISPLAY environment
 *  variable. Otherwise, if the environment variable is set use:
 *  char *display_name = getenv("DISPLAY");
 */
constexpr std::string_view display_name{"localhost::0.0"};

/**
 * @brief Dimensions of our clock window. For the moment we will
 * initialise these from the global constexprs in dimensions.h
 */
constexpr size_t clock_window_width{WIDTH};
constexpr size_t clock_window_height{HEIGHT};

/**
 * @brief frames_per_second controls the timing of the event loop using
 *  nanosleep
 */
constexpr int frames_per_second{1};
using namespace std::chrono_literals;
constexpr auto frame_period{std::chrono::nanoseconds(1s) / frames_per_second};

/**
 * @brief Flags to limit our event subscriptions to expose events
 * This means we don't get keyboard or pointer events
 */
constexpr auto expose_flags{ExposureMask | StructureNotifyMask};

using std::cout;
using std::endl;
using std::pair;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

class x11Driver : public displayDriver
{
public:
    explicit x11Driver();
    void clear();
    void set(int x, int y);
    void update() override;
    ~x11Driver();

private:
    bool x11_setup();
    pair<bool, Display *> open_display(std::string_view name);
    pair<bool, int> get_default_screen(Display *d);
    pair<bool, GC> create_gc(Display *d, Window w);
    pair<bool, Visual *> get_default_visual(Display *d, int sn);
    pair<bool, Window> create_window(Display *d, int screen, int width, int height, Point tl);
    pair<bool, XImage *> create_image(Display *d, Visual *v, int width, int height);
    pair<bool, GC> set_gc(Display *d, int sn, Window w, GC &gc);
    bool subscribe_events(Display *d, Window w, unsigned long event_flags);
    bool map_and_flush();

    bool start_event_loop();
    void put_image();
    void wait(std::chrono::nanoseconds nano);
    TimePoint last_period(TimePoint last);
    bool eventLoop();

    /**
     * @brief Controls the event loop
     * True - event loop runs
     * False - event loop terminates
     */
    bool run;
    /**
     * @brief Future for the X11 event loop that we start with a call
     * to async
     */
    std::future<bool> el;
    /**
     * @brief Our display structure - uses default if no display_name is given
     */
    Display *display;
    /**
     * @brief A pointer to the opaque X11 struct defining the graphics structure of the display
     * (StaticGray, GrayScale, StaticColor, PseudoColor, TrueColor, or DirectColor). Obtained
     * with: Visual *XDefaultVisual(Display *display, int screen_number);
     */
    Visual *visual;
    /**
     * @brief The window we create to draw the clock
     */
    Window window;
    /**
     * @brief The (integer) number of the screen for the clock window (not the Screen struct)
     */
    int screen_no;
    /**
     * @brief The graphics context struct GC used for drawing the clok
     */
    GC context;
    /**
     * @brief Geometry of the screen we are using
     */
    uint_t screen_width, screen_height;

    /**
     * @brief Top left corner of the xclock window
     */
    Point win_top_left;
    /**
     * @brief The image structure (data and metadata) that we use to transfer the
     * data pointed to in the update function to X11 clock window
     */
    XImage *image;
    /**
     * @brief Where we store the pointer passed by the update member function.
     * In this version we're using a local copy but it would be better to use
     * the buffer defined in the calling program (but we don't have this when
     * we're instantiated).
     */
    char data[clock_window_width * clock_window_height];
};
