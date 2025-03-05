#include "display.hpp"
#include <string.h>

Display::Display(uint8_t sda_pin, uint8_t scl_pin, uint32_t clock, uint8_t height, uint8_t width)
{
  this->sda_pin = sda_pin;
  this->scl_pin = scl_pin;
  this->clock = clock;
  this->height = height;
  this->width = width;
}

void Display::setup()
{
  i2c_init(i2c1, clock);
  gpio_set_function(sda_pin, GPIO_FUNC_I2C);
  gpio_set_function(scl_pin, GPIO_FUNC_I2C);
  gpio_pull_up(sda_pin);
  gpio_pull_up(sda_pin);

  this->disp.external_vcc = false;
  ssd1306_init(&disp, height, width, 0x3C, i2c1);
  clear_display();
  y = 0;
}

void Display::draw_string(uint8_t x, const char *message, uint8_t scale = 1)
{
  ssd1306_draw_string(&disp, x, y, scale, message);
  y = y + 8 > disp.height ? 0 : y + 8;
  ssd1306_show(&disp);
}

void Display::draw_line(uint8_t x1, uint8_t x2, uint8_t y1 = 0, uint8_t y2 = 0)
{
  if (y1 == 0 && y2 == 0)
    ssd1306_draw_line(&disp, x1, x2, y, y);
  else
    ssd1306_draw_line(&disp, x1, x2, y1, y2);
  ssd1306_show(&disp);
}

void Display::clear_display()
{
  memset(disp.buffer, 0, disp.bufsize);
  ssd1306_show(&disp);
}