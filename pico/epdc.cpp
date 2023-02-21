
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include <iostream>
#include <string>
#include <ctime>

#include "dimensions.h"
#include "errorCodes.h"
#include "ntpClient.h"
#include "topCat.h"
#include "layoutServer.h"
#include "uc8151.h"
#include "debug.h"

static volatile bool ntp_refresh;

std::ostream &operator<<(std::ostream &os, datetime_t dt)
{
    os << dt.hour << ":" << dt.min << dt.sec;
    return os;
}

void alrmCallback()
{
    ntp_refresh = true;
}

int wifiConnected()
{
    if (auto err{cyw43_arch_init()}; err != pico_error::code::PICO_OK)
    {
        dbg("Failed to initialise cyw43 (" << pico_error::toString(err) << ")" << std::endl);
        return err;
    }
    cyw43_arch_enable_sta_mode();

    if (auto err{
            cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)};
        err != pico_error::code::PICO_OK)
    {
        dbg("Connect timeout (err=" << err << ")" << std::endl);
        return err;
    }
    else
    {
        dbg("Connected to " << WIFI_SSID << "\n\r" << std::endl);
    }
    return PICO_OK;
}

int wifiDisconnected()
{
    cyw43_arch_deinit();
    dbg("Deinitialising cyw43" << std::endl);
    return 0;
}

ntp_result_t ntpLoop()
{
    ntpClient client;
    return client.getNTPTime();
}

int ntp2rtc()
{
    auto [status, time]{ntpLoop()};
    if (status == PICO_OK)
    {
        datetime_t starttime;
        if (TopCat::toDateTime(&time, &starttime))
        {
            rtc_set_datetime(&starttime);
            sleep_ms(10);
        }
        datetime_t setTime;
        rtc_get_datetime(&setTime);
        status = (setTime.hour == starttime.hour) && (setTime.min == starttime.min);
        if (!status)
        {
            dbg(setTime.hour << ":" << setTime.min << " != " << starttime.hour << ":" << starttime.min << std::endl);
        }
    }
    return status;
}

bool rtc_started()
{
    rtc_init();
    dbg("Initialised RTC...\n");
    // Arbitrary default start of Sunday 19 Feb 23 09:00:00
    datetime_t t = {
        .year = 2023,
        .month = 02,
        .day = 19,
        .dotw = 0,
        .hour = 9,
        .min = 00,
        .sec = 00};

    rtc_set_datetime(&t);
    sleep_ms(10);

    if (!rtc_running())
    {
        return false;
    }

    auto timeIsSet{ntp2rtc()};
    int retryLimit{3};
    while (!timeIsSet && retryLimit != 0)
    {
        dbg("Couldn't set time from NTP retrying in 5 seconds" << std::endl);
        sleep_ms(5000);
        timeIsSet = ntp2rtc();
        retryLimit--;
    }

    if (!timeIsSet)
    {
        return false;
    }

    // Alarm once a day
    datetime_t alarm{TopCat::day_alarm()};
    rtc_set_alarm(&alarm, alrmCallback);
    return rtc_running();
}

void printTime(datetime_t *dt)
{
    char datetime_buf[256];
    char *datetime_str = &datetime_buf[0];
    rtc_get_datetime(dt);
    datetime_to_str(datetime_str, sizeof(datetime_buf), dt);
    dbg("RTC time is " << datetime_str << "          " << std::endl);
}

int main()
{
    stdio_init_all();

    if (wifiConnected() != PICO_OK)
    {
        dbg("Unable to connect to wifi - exiting" << std::endl);
        return EXIT_FAILURE;
    }

    if (!rtc_started())
    {
        dbg("Unable to start RTC - exiting" << std::endl);
        return EXIT_FAILURE;
    }

    // Create a unq ptr to the driver. This is how we use same the
    // library classes with the epd and with X11 (see desktop.cpp for the X11 app)
    auto driver{std::make_unique<UC8151>()};
    LayoutServer lo(std::move(driver));

    ntp_refresh = true;
    datetime_t startTime;

    // This is only used if we want to track the drift between the pico rtc and ntp
    // it sets the epoch of the Topcat object at startup (only)
    rtc_get_datetime(&startTime);
    TopCat topcat(startTime);
    dbg("Starting epdc at " << topcat.epoch().hour << ":" << topcat.epoch().min << std::endl);

    while (true)
    {
        datetime_t newTime;
        if (ntp_refresh)
        {
            auto [status, ntpTime]{ntpLoop()};
            if (status == PICO_OK)
            {
                if (TopCat::toDateTime(&ntpTime, &newTime))
                {
                    ntp_refresh = false;
                    datetime_t oldTime;
                    rtc_get_datetime(&oldTime);
                    rtc_set_datetime(&newTime);
                    dbg("Drift is " << topcat.elapsed(newTime) << "s since " << topcat.epoch() << std::endl);
                };
            }
        }
        rtc_get_datetime(&newTime);
        lo.timeIs(newTime);
        sleep_ms(1000 * 60);
    }

    return EXIT_SUCCESS;
}