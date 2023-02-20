
#pragma once

#include <cinttypes>
#include <algorithm>
#include <string_view>

/**
 * @brief Typedef for the length of the unsigned intergers we're using
 * 
 */
typedef uint16_t uint_t;

enum class driverType
{
    ascii,
    x11,
    epd
};

/**
 * @brief constexpr to determine the type of display driver for this build
 * 
 */
constexpr driverType driver{driverType::epd};

/**
 * @brief Widriverh in pixels of the display
 */
constexpr uint_t WIDTH{driver==driverType::epd? 296: driver==driverType::ascii ? 64 : 600};

/**
 * @brief Height in pixels of the EPD
 */
constexpr uint_t HEIGHT{driver==driverType::epd? 128: driver==driverType::ascii ? 64 : 400};

constexpr int MAX_TEXT_LEN{1024};
constexpr char CR{'\r'};
constexpr char LF{'\n'};
constexpr char SPACE{' '};
constexpr char EOT{0};
constexpr char wspace[4]{CR, LF, SPACE, EOT};
constexpr std::string_view WHITESPACE(wspace, std::size(wspace));
constexpr char ERROR_CHAR{'#'};

// Layout values
constexpr int marginV{2};
constexpr int marginH{0};

/**
 * @brief We check the sign bit in clamp* to prevent -1 being interpreted as
 * a uint16_t of 65,535
 *
 */
constexpr uint_t signBit{1U << 15};

/**
 * @brief Capacity of the buffer needed for HEIGHT x WIDTH
 */
constexpr uint_t FRAMEBUFFERSIZE{(HEIGHT * WIDTH / 8)};

/**
 * @brief Maximum value of an index into a buffer of HEIGHT x WIDTH
 */
constexpr uint_t MAX_INDEX{FRAMEBUFFERSIZE - 1};

/**
 * @brief Maximium value of a vertical (y) coordinate
 */
constexpr uint_t MAX_Y_BITS{HEIGHT - 1};

/**
 * @brief Maximum value of a byte coordinate
 */
constexpr uint_t MAX_Y_BYTE{(HEIGHT / 8) - 1};

/**
 * @brief Maximum value of a horizontal (x) coordinate
 */
constexpr uint_t MAX_X{WIDTH - 1};

/**
 * @brief Maximum size of a message in bytes
 *  based on the number of characters that can be displayed in an 8x8 font
 */
constexpr uint_t MAX_MSG_LEN{HEIGHT * WIDTH / (8 * 8)};
