#pragma once

#include "bdfFont.h"

struct Style
{
    BdfFont font;
    BdfFont bold;
    int homeX;
    int homeY;
    int topMargin;
    int leftMargin;
    int originX;
    int originY;
    int rowMargin;
};
