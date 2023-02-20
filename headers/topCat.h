#pragma once

#include <cinttypes>
#include <tuple>
#include <iostream>
#include <ctime>
#include <chrono>
#include "picoDatetime.h"

/**
 * @brief converts a standard C time_t (i.e. seconds since 1970) to a Pico dateTime_t or
 * a standard C tm struct (broken out time).
 */
class TopCat
{
public:
    TopCat(datetime_t startupTime);
    static time_t timeNow();
    time_t elapsed(const datetime_t dt);
    inline datetime_t epoch() { return startup; };
    static bool toTm(time_t const *tt, tm *ttm);
    static bool toTm(datetime_t const *dt, tm *ttm);
    static bool toDateTime(tm const *ttm, datetime_t *dt);
    static bool toDateTime(const time_t *tt, datetime_t *dt);
    static datetime_t minute_alarm();
    static datetime_t hour_alarm();
    static datetime_t day_alarm();
    static std::string toString(const datetime_t *dt);
    static std::string toClockTime(datetime_t &dt);
    static std::string toClockTime(const tm &ttm);

private:
    datetime_t startup;
    time_t picoch;

    /**
     * @brief returns the number of the last day in the given month
     *
     * @param month - the month to check
     * @return uint8_t - the last day in the month (where first day is 1), or zero if month is out of range
     */
    static uint8_t lastOfMonth(const uint8_t month);

    /**
     * @brief Check the validity of the local datetime_t struct
     *
     */
    static bool dtIsValid(datetime_t const *pt);

    /**
     * @brief Copies the given dm struct into the datetime_t struct without error checking
     * @param tt - the tm struct to be copied
     * @param utm - a reference to the receiving datetime_t struct
     */
    static void rawCopy_dt(tm const *tmt, datetime_t *dt);

    static datetime_t default_dt();

    static inline void setGBLocale() { setlocale(LC_TIME, "en_GB.utf8"); };
};

std::ostream &operator<<(std::ostream &os, datetime_t dt);