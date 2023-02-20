#pragma once

#include "bdfFont.h"
#include "font112.h"
#include "sans22.h"

Style clockStyle{
    .font = font112,
    .homeX = 0,
    .homeY = 10,
    .topMargin = 0,
    .leftMargin = 0,
    .originX = 0,
    .originY = 0,
    .rowMargin = 0};

Style quoteStyle{
    .font = Sans22,
    .homeX = 0,
    .homeY = 10,
    .topMargin = 0,
    .leftMargin = 0,
    .originX = 0,
    .originY = 0,
    .rowMargin = 1};