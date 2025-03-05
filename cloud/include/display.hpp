#ifndef DISPLAY_INTERFACE_HPP
#define DISPLAY_INTERFACE_HPP

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

class Display
{
protected:
  uint8_t sda_pin;
  uint8_t scl_pin;
  uint8_t height;
  uint8_t width;
  uint32_t clock;
  ssd1306_t disp;
  uint8_t y;

public:
  Display(uint8_t sda_pin, uint8_t scl_pin, uint32_t clock, uint8_t height, uint8_t width);
  void setup();
  void draw_string(uint8_t x, const char *message, uint8_t scale);
  void draw_line(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2);
  void clear_display();
};

#endif // DISPLAY_INTERFACE_HPP