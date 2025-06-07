#pragma once

#include "bdfFont.h"
#include "font112.h"
#include "sans22.h"
#include "sans24.h"

constexpr char boldOn = '<';
constexpr char boldOff = '>';

Style clockStyle{
    .font = font112,
    .bold = Sans24,
    .homeX = 0,
    .homeY = 10,
    .topMargin = 0,
    .leftMargin = 0,
    .originX = 0,
    .originY = 0,
    .rowMargin = 0};

Style quoteStyle{
    .font = Sans22,
    .bold = Sans24,
    .homeX = 0,
    .homeY = 10,
    .topMargin = 0,
    .leftMargin = 0,
    .originX = 0,
    .originY = 0,
    .rowMargin = 1};