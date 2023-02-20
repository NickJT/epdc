#include "topCat.h"

TopCat::TopCat(datetime_t startupTime) : picoch{0}
{
    if (dtIsValid(&startupTime))
    {
        startup = startupTime;
    }
    else
    {
        startup = default_dt();
    }

    tm startup_tm;
    if (toTm(&startup, &startup_tm))
    {
        picoch = mktime(&startup_tm);
    }
}

time_t TopCat::elapsed(const datetime_t dt)
{
    tm now_tm;
    time_t now_t{0};
    if (toTm(&dt, &now_tm))
    {
        now_t = mktime(&now_tm);
    }
    return now_t - picoch;
}

/**
 * @brief Converts a time_t to a broken out tm in the GB time zone
 *
 * @param tt - the gmt time_t
 * @return status_tm - status or a tm in the GB timezone
 */
bool TopCat::toTm(time_t const *tt, tm *ttm)
{
    setGBLocale();
    // localtime_r return the buffer pointer on sucess or null pointer otherwise
    tm *localtm{localtime_r(tt, ttm)};
    return (localtm == ttm);
}

/**
 * @brief Converts a Pico datetime_t into a tm
 *
 * @param tt
 * @param ttm
 * @return true
 * @return false
 */
bool TopCat::toTm(datetime_t const *dt, tm *ttm)
{
    if (!dtIsValid(dt))
    {
        return false;
    }
    ttm->tm_year = dt->year - 1900;
    ttm->tm_mon = dt->month - 1; /// re-base to 0
    ttm->tm_mday = dt->day;
    ttm->tm_wday = dt->dotw;
    ttm->tm_hour = dt->hour;
    ttm->tm_min = dt->min;
    ttm->tm_sec = dt->sec;
    return true;
}

bool TopCat::toDateTime(tm const *ttm, datetime_t *dt)
{
    rawCopy_dt(ttm, dt);
    auto res = dtIsValid(dt);
    return res;
}

bool TopCat::toDateTime(const time_t *tt, datetime_t *dt)
{
    tm ttm;
    setGBLocale();
    bool ok{false};
    if (toTm(tt, &ttm))
    {
        ok = toDateTime(&ttm, dt);
    }
    return ok;
}

std::string TopCat::toString(const datetime_t *dt)
{
    tm ttm;
    if (!toTm(dt, &ttm))
    {
        return "invalid datetime_t";
    };
    std::string stg{asctime(&ttm)};
    stg.resize(stg.size() - 1);
    return stg;
}

datetime_t TopCat::minute_alarm()
{
    datetime_t t = {
        .year = -1,
        .month = -1,
        .day = -1,
        .dotw = -1, // 0 is Sunday, so 5 is Friday
        .hour = -1,
        .min = -1,
        .sec = 0};
    return t;
}

datetime_t TopCat::hour_alarm()
{
    datetime_t t = {
        .year = -1,
        .month = -1,
        .day = -1,
        .dotw = -1, // 0 is Sunday, so 5 is Friday
        .hour = -1,
        .min = 56,
        .sec = -1};
    return t;
}

datetime_t TopCat::day_alarm()
{
    datetime_t t = {
        .year = -1,
        .month = -1,
        .day = -1,
        .dotw = -1, // 0 is Sunday, so 5 is Friday
        .hour = 02,
        .min = 02,
        .sec = -1};
    return t;
}

/**
 * @brief Generates a five digit clock face HH:MM
 * @param ttm - A standard tm struct
 * @return std::string showing HH:MM
 */
std::string TopCat::toClockTime(const tm &ttm)
{
    char str[]{'h', 'h', ':', 'm', 'm', 0};
    // auto res{strftime(str, sizeof(str), "%R", &ttm)};
    if (strftime(str, sizeof(str), "%R", &ttm))
    {
        std::string{str};
        return str;
    }
    return "invalid tm";
}

time_t TopCat::timeNow()
{
    auto now{std::chrono::system_clock::now()};
    time_t timeNow{std::chrono::system_clock::to_time_t(now)};
    return timeNow;
}

/**
 * @brief Generates a five digit clock face HH:MM
 * @param dt - A pico datetime
 * @return std::string showing HH:MM
 */
std::string TopCat::toClockTime(datetime_t &dt)
{
    tm ttm;
    if (toTm(&dt, &ttm))
    {
        return toClockTime(ttm);
    }
    return "invalid datetime_t";
}

/**
 * @brief returns the number of the last day in the given month
 * @param month
 * @return uint8_t - the last day in @param month (where first day is 1), or zero if month is out of range
 */
uint8_t TopCat::lastOfMonth(uint8_t month)
{
    const int8_t lastDay[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 13)
    {
        return lastDay[month];
    }
    return lastDay[0];
}

/**
 * @brief Check the validity of a datetime_t struct
 */
bool TopCat::dtIsValid(datetime_t const *dt)
{
    bool fail{dt->year < 0};
    fail |= dt->month < 1 || dt->month > 12;
    fail |= dt->day < 1 || dt->day > lastOfMonth(dt->month);
    fail |= dt->dotw < 0 || dt->dotw > 6; // 0 is Sunday
    fail |= dt->hour < 0 || dt->hour > 23;
    fail |= dt->min < 0 || dt->min > 59;
    fail |= dt->sec < 0 || dt->sec > 59;
    return !fail;
}

void TopCat::rawCopy_dt(tm const *tmt, datetime_t *dt)
{
    dt->year = tmt->tm_year + 1900;
    dt->month = tmt->tm_mon + 1; /// re-base to 1 so 1..12, 1 is January
    dt->day = tmt->tm_mday;
    dt->dotw = tmt->tm_wday; // 0 is Sunday
    dt->hour = tmt->tm_hour;
    dt->min = tmt->tm_min;
    dt->sec = tmt->tm_sec;
}

datetime_t TopCat::default_dt()
{
    datetime_t ddt;
    ddt.hour = 0;
    ddt.min = 0,
    ddt.sec = 0;

    ddt.dotw = 0; // - is Sunday
    ddt.day = 1;
    ddt.month = 1;
    ddt.year = 2023;
    return ddt;
}
