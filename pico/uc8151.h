#pragma once
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <climits>
#include <cinttypes>
#include <cstddef>
#include "dimensions.h"
#include "geometry.h"
#include "frameBuffer.h"

class UC8151 : public displayDriver
{
public:
    explicit UC8151();
    void update() override;
    void clear() override;
    void set(int x, int y) override;
    ~UC8151() override {};

private:
    void initPico();
    void initDriver();
    void command(uint8_t reg, size_t len, const uint8_t *data);
    void command(uint8_t reg, std::initializer_list<uint8_t> values);
    void command(uint8_t reg) { command(reg, 0, nullptr); };
    void data(size_t len, const uint8_t *data);
    void off();
    void read(uint8_t reg, size_t len, uint8_t *data);
    void busy_wait();
    void reset();
    bool is_busy();
    void power_off();
    bool set_update_speed(int update_speed);

    void default_luts();
    void medium_luts();
    void fast_luts();
    void turbo_luts();
    uint32_t update_time();
    uint8_t get_update_speed();
    uint8_t update_speed;

    FrameBuffer buffer;

    bool inverted{false};
    bool blocking{true};

    // interface pins (from inkyPack Schematic)
    uint CS{17};
    uint DC{20};
    uint SCK{18};
    uint MOSI{19};
    uint busy{26};
    uint resetPin{21};
};