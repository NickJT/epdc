
#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include "x11Driver.h"
#include "layoutServer.h"

using namespace std;

int main()
{
    char *display_name = getenv("DISPLAY");
    cout <<  "DISPLAY = " << display_name << endl;

    auto driver{std::make_unique<x11Driver>()};
    LayoutServer lo(std::move(driver));
    //int earlyBath{3};
    while (true)
    {
        const time_t now{TopCat::timeNow()};
        cout << "Time is " << ctime(&now);
        datetime_t dt;
        if (TopCat::toDateTime(&now, &dt))
        {
            lo.timeIs(dt);
        }
        std::this_thread::sleep_for(5s);
    }
    std::cout << "Terminating event loop" << std::endl;
    return 0;
}