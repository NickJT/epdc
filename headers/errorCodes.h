#pragma once
#include <string>

class pico_error
{
public:
    enum code
    {
        PICO_OK = 0,
        PICO_ERROR_NONE = 0,
        PICO_ERROR_TIMEOUT = -1,
        PICO_ERROR_GENERIC = -2,
        PICO_ERROR_NO_DATA = -3,
        PICO_ERROR_NOT_PERMITTED = -4,
        PICO_ERROR_INVALID_ARG = -5,
        PICO_ERROR_IO = -6
    };

    static std::string toString(int e)
    {
        switch (e)
        {
        case static_cast<int>(code::PICO_OK):
            return "PICO_OK";
            break;
        case static_cast<int>(code::PICO_ERROR_TIMEOUT):
            return "PICO_ERROR_TIMEOUT";
            break;
        case static_cast<int>(code::PICO_ERROR_GENERIC):
            return "PICO_ERROR_GENERIC";
            break;
        case static_cast<int>(code::PICO_ERROR_NO_DATA):
            return "PICO_ERROR_NO_DATA";
            break;
        case static_cast<int>(code::PICO_ERROR_NOT_PERMITTED):
            return "PICO_ERROR_NOT_PERMITTED";
            break;
        case static_cast<int>(code::PICO_ERROR_INVALID_ARG):
            return "PICO_ERROR_INVALID_ARG";
            break;
        case static_cast<int>(code::PICO_ERROR_IO):
            return "PICO_ERROR_IO";
            break;
        default:
            return "Unknown code";
            break;
        }
    }
};