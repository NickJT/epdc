
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"

#include "dimensions.h"
#include "errorCodes.h"
#include "ntpClient.h"
#include "topCat.h"
#include "layoutServer.h"
#include "uc8151.h"
#include <iostream>
#include <string>
#include <ctime>

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
        std::cout << "Failed to initialise cyw43 (" << pico_error::toString(err) << ")" << std::endl;
        return err;
    }
    cyw43_arch_enable_sta_mode();

    if (auto err{
            cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)};
        err != pico_error::code::PICO_OK)
    {
        std::cout << "Connect timeout (err=" << err << ")" << std::endl;
        return err;
    }
    else
    {
        std::cout << "Connected to " << WIFI_SSID << "\n\r" << std::endl;
    }
    return PICO_OK;
}

int wifiDisconnected()
{
    cyw43_arch_deinit();
    std::cout << "Deinitialising cyw43" << std::endl;
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
            std::cout << setTime.hour << ":" << setTime.min << " != " << starttime.hour << ":" << starttime.min << std::endl;
        }
    }
    return status;
}

bool rtc_started()
{
    rtc_init();
    std::cout << "Initialised RTC...\n";
    // Start on Friday 5th of June 2020 15:45:00
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
        std::cout << "Couldn't set time from NTP retrying in 5 seconds" << std::endl;
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
    std::cout << "RTC time is " << datetime_str << "          " << std::endl;
}

int main()
{
    stdio_init_all();

    if (wifiConnected() != PICO_OK)
    {
        std::cout << "Unable to connect to wifi - exiting" << std::endl;
        return EXIT_FAILURE;
    }

    if (!rtc_started())
    {
        std::cout << "Unable to start RTC - exiting" << std::endl;
        return EXIT_FAILURE;
    }

    auto driver{std::make_unique<UC8151>()};
    LayoutServer lo(std::move(driver));
    ntp_refresh = true;
    datetime_t startTime;
    rtc_get_datetime(&startTime);
    TopCat topcat(startTime);
    std::cout << "Starting epdc at " << topcat.epoch().hour << ":" << topcat.epoch().min << std::endl;
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
                    std::cout << "Drift is " << topcat.elapsed(newTime) << "s since " << topcat.epoch() << std::endl;
                };
            }
        }
        rtc_get_datetime(&newTime);
        lo.timeIs(newTime);
        sleep_ms(1000 * 60);
    }

    return EXIT_SUCCESS;
}