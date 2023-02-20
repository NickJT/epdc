#pragma once

#include <cinttypes>
#include <iostream>
#include <span>
#include "dimensions.h"

class displayDriver
{
public:
    virtual void clear() = 0;
    virtual void set(int x, int y) = 0;
    virtual void update() = 0;
    virtual ~displayDriver();
};
