#pragma once
#include <iostream>
#include <iomanip>

#ifdef PICO_BOARD

#include "pico/util/datetime.h"

#else

#include <cinttypes>

typedef struct
{
    int16_t year; ///< 0..4095
    int8_t month; ///< 1..12, 1 is January
    int8_t day;   ///< 1..28,29,30,31 depending on month
    int8_t dotw;  ///< 0..6, 0 is Sunday
    int8_t hour;  ///< 0..23
    int8_t min;   ///< 0..59
    int8_t sec;   ///< 0..59
} datetime_t;

#endif