#include "x11Driver.h"

/*
display Specifies the connection to the X server.
visual Specifies the Visual structure.
depth Specifies the depth of the image.
format Specifies the format for the image. You can pass XYBitmap, XYPixmap, or ZPixmap.
offset Specifies the number of pixels to ignore at the beginning of the scanline.
data Specifies the image data.
width Specifies the width of the image, in pixels.
height Specifies the height of the image, in pixels.
bitmap_pad Specifies the quantum of a scanline (8, 16, or 32). In other words, the start of one scanline is separated in client memory from the start of the next scanline by an integer multiple of this many bits.
bytes_per_line Specifies the number of bytes in the client image between the start of one scanline and the start of the next.
*/

x11Driver::x11Driver() : run{true}, el{}, display{nullptr}, window{NULL}, screen_no{0}, context{nullptr},
                         screen_width{0}, screen_height{0},
                         win_top_left{0, 0}, data{}
{
    std::cout << "x11Driver starting - " << frames_per_second << "fps => " << frame_period.count() << "nS per frame" << std::endl;
    auto setup_complete{x11_setup()};
    if (!setup_complete)
    {
        std::cout << "X11 setup failed" << std::endl;
        assert(setup_complete);
    }
    map_and_flush();
    start_event_loop();
}

void x11Driver::clear()
{
    std::cout << "Clearing\n\r";
    memset(data,0,sizeof(data));
}

void x11Driver::set(int x, int y)
{
    std::cout << "(" << x << "," << y << ") ";
    XPutPixel(image,x,y,0xFF);
}

// For testing we'll just copy the data locally...
void x11Driver::update()
{
    std::cout << "\n\rx11 update";
    std::cout << std::endl;
}

bool x11Driver::x11_setup()
{
    std::cout << "x11 setup underway \n\r";
    if (auto [ok, d]{open_display(display_name)}; ok)
    {
        display = d;
    }
    else
    {
        cout << "Can't open " << display_name << "\n\r";
        return false;
    }

    if (auto [ok, sn]{get_default_screen(display)}; ok)
    {
        screen_no = sn;
    }
    else
    {
        cout << "Can't get default screen for display " << display_name << "\n\r";
        return false;
    }

    if (auto [ok, w]{create_window(display, screen_no, clock_window_width, clock_window_height, win_top_left)}; ok)
    {
        window = w;
    }
    else
    {
        cout << "Can't define X11 window \n\r";
        return false;
    }

    if (auto [ok, gc]{create_gc(display, window)}; ok)
    {
        context = gc;
    }
    else
    {
        cout << "Can't create context\n\r";
    }

    if (auto [ok, v]{get_default_visual(display, screen_no)}; ok)
    {
        visual = v;
    }
    else
    {
        cout << "Can't get default visual for display " << display_name << "\n\r";
        return false;
    }

    if (auto [ok, img]{create_image(display, visual, clock_window_width, clock_window_height)}; ok)
    {
        image = img;
    }
    else
    {
        cout << "Can't allocate image structure \n\r";
        return false;
    }
    XStoreName(display, window, "X11 Book Clock");

    subscribe_events(display, window, expose_flags);

    return true;
}

pair<bool, Display *> x11Driver::open_display(std::string_view name)
{
    Display *d{NULL};
    if (name.empty())
    {
        d = XOpenDisplay(NULL);
    }
    else
    {
        d = XOpenDisplay(NULL);
    }

    if (d != NULL)
    {
        cout << "Opened " << display_name << endl;
        return {true, d};
    }
    cout << "Can't connect to X server on " << display_name << endl;
    return {false, NULL};
}

pair<bool, int> x11Driver::get_default_screen(Display *d)
{
    if (d == NULL)
    {
        return {false, 0};
    }
    int sn{XDefaultScreen(d)};
    return {true, sn};
}

/**
 * @brief Creates a graphics context defined by the passed struct
 *
 * @param d - Display struct*
 * @param sn - Screen number
 * @return pair<bool, GC> - True if the gc is valid
 */
pair<bool, GC> x11Driver::create_gc(Display *d, Window w)
{
    if (d == nullptr)
    {
        return {false, {}};
    }

    /* Struct and mask to control which of the GC values we are changing
        not used here because we use the convenience functions */
    unsigned long valuemask{0};
    XGCValues values{};

    GC gc{XCreateGC(d, w, valuemask, &values)};
    if (gc == nullptr)
    {
        return {false, {}};
    }

    /* We're setting a lot of values here which won't actually be used because
       all we are going to display is the image */
    unsigned int line_width{1};
    int line_style{LineSolid};
    int cap_style{CapButt};
    int join_style{JoinBevel};
    XSetLineAttributes(d, gc, line_width, line_style, cap_style, join_style);
    XSetFillStyle(d, gc, FillSolid);
    XSetForeground(d, gc, WhitePixel(d, screen_no));
    XSetBackground(d, gc, BlackPixel(d, screen_no));
    return {true, gc};
}

/**
 * @brief Applies some hardcoded settings to the passed GC
 *
 * @param d - Display struct*
 * @param w - Window struct
 * @return true - True if the gc was applied successfully
 * @return false - If any of the system called errored
 */
pair<bool, GC> x11Driver::set_gc(Display *d, int sn, Window w, GC &gc)
{
    if (display == nullptr || gc == nullptr)
    {
        return {false, {}};
    }
    unsigned int line_width{1};
    int line_style{LineSolid};
    int cap_style{CapButt};
    int join_style{JoinBevel};
    XSetLineAttributes(d, gc, line_width, line_style, cap_style, join_style);
    XSetFillStyle(d, gc, FillSolid);
    XSetBackground(d, gc, BlackPixel(d, sn));
    XSetForeground(d, gc, WhitePixel(d, sn));
    return {true, gc};
}

bool x11Driver::subscribe_events(Display *d, Window w, unsigned long event_flags)
{
    XSelectInput(d, w, event_flags);
    return true;
}

/**
 * @brief A wrapper for XDefaultVisual(Display *display, int screen_number). Not really
 *  necessary because the X11 function doesn't provide Status so all we can do is a null check
 * @param d - Display
 * @param sn - Screen number
 * @return pair<bool, Visual> A pointer to the visual structure of the display
 */
pair<bool, Visual *> x11Driver::get_default_visual(Display *d, int sn)
{
    if (d == nullptr)
    {
        return {false, nullptr};
    }
    auto vis{XDefaultVisual(d, sn)};
    return {vis != nullptr, vis};
}

pair<bool, Window> x11Driver::create_window(Display *d, int screen, int width, int height, Point tl)
{
    constexpr int border_width{2};
    auto white{XWhitePixel(display, screen)};
    auto black{XBlackPixel(display, screen)};
    auto win{XCreateSimpleWindow(d, XRootWindow(d, screen),
                                 tl.xVal(), tl.yVal(),
                                 width, height,
                                 border_width,
                                 white, black)};
    if (win)
    {
        return {true, win};
    }
    return {false, NULL};
}

/*
 * @brief Wrapper function for XCreateImage - Initialises the struct that transfers image
 data from here to the X11 server
 XImage *XCreateImage(Display *display, Visual *visual, unsigned int depth,
 int format, int offset, char *data, unsigned int width, unsigned int height,
 int bitmap_pad, int bytes_per_line);

display Specifies the connection to the X server.
visual  Specifies the Visual structure.
depth   Specifies the depth of the image.
format  Specifies the format for the image. You can pass XYBitmap, XYPixmap, or ZPixmap.
offset  Specifies the number of pixels to ignore at the beginning of the scanline.
data    Specifies the image data.
width   Specifies the width of the image, in pixels.
height  Specifies the height of the image, in pixels.

bitmap_pad Specifies the quantum of a scanline (8, 16, or 32). In other words,
the start of one scanline is separated in client memory from the start of the
next scanline by an integer multiple of this many bits.

bytes_per_line Specifies the number of bytes in the client image between the start of one
scanline and the start of the next.
*/
pair<bool, XImage *> x11Driver::create_image(Display *d, Visual *v, int width, int height)
{
    int depth = 1;
    int offset{0};
    int bitmap_pad{8};
    int bytes_per_line{width / 8};
    auto img{XCreateImage(display, visual, depth, XYBitmap,
                          offset, data, width, height,
                          bitmap_pad, bytes_per_line)};
    return {img != nullptr, img};
}

bool x11Driver::map_and_flush()
{
    /*Show the window on the screen at the top of the stack*/
    XMapRaised(display, window);
    XFlush(display);
    return true;
}

bool x11Driver::start_event_loop()
{
    el = std::async(&x11Driver::eventLoop, this);
    return el.valid();
}

/*
XPutImage(Display *display, Drawable d, GC gc, XImage *image, int src_x, int src_y, int dest_x, int dest_y, unsigned int width, unsigned int height);

display Specifies the connection to the X server.
d   Specifies the drawable.
gc  Specifies the GC.
image   Specifies the image you want combined with the rectangle.
src_x   Specifies the offset in X from the left edge of the image defined by the XImage structure.
src_y   Specifies the offset in Y from the top edge of the image defined by the XImage structure.
dest_x  Specify the x and y coordinates, which are relative to the origin of the drawable and are the coordinates of the subimage.
dest_y
width   Specify the width and height of the subimage, which define the dimensions of the rectangle.
height
*/
void x11Driver::put_image()
{
    if (!(display == nullptr || context == nullptr || image == nullptr))
    {
        XPutImage(display, window, context, image, 0, 0, 0, 0,
                  clock_window_width, clock_window_height);
    }
    else
    {
        cout << "Null value in call to XPutImage\n\r";
    }
}

void x11Driver::wait(std::chrono::nanoseconds nano)
{
    std::this_thread::sleep_for(nano);
}

TimePoint x11Driver::last_period(TimePoint last)
{
    auto now{std::chrono::system_clock::now()};
    std::chrono::nanoseconds period{now - last};
    if (period < frame_period)
    {
        std::this_thread::sleep_for(frame_period - period);
    }
    return std::chrono::system_clock::now();
}

/**
 * @brief X11 event loop
 *
 * @return true - If we terminated because the class set run to false (i.e. intended)
 * @return false - If we terminated because of an X11 error (or something else we weren't expecting)
 */
bool x11Driver::eventLoop()
{
    auto earlier{std::chrono::system_clock::now()};
    put_image();
    XFlush(display);
    while (run)
    {
        cout << "Tick\n\r";
        XEvent event;
        put_image();
        while (XPending(display))
        {
            XNextEvent(display, &event);
            switch (event.type)
            {
            case Expose:
                cout << "Expose event\n\r";
                if (event.xexpose.count == 0)
                {
                    put_image();
                    XFlush(display);
                }
                break;
            default:
                break;
            }
        }
        earlier = last_period(earlier);
    }
    std::cout << "Event loop terminating gracefully\n\r";
    return !run;
}

x11Driver::~x11Driver()
{
    run = false;
    std::cout << "~~x11Driver destructor~~\n\r";
    auto result{el.get()};
    if (!result)
    {
        std::cout << " Event Loop returned false\n\r";
    }
    // the el.get() above should block until the event loop thread
    // has ended so we can now delete the x11 structures
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    cout << "X11 clean-up completed\n\r";
}
