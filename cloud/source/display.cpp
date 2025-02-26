#include "display.hpp"

Display::Display(uint8_t sda_pin, uint8_t scl_pin, uint32_t clock)
{
  this->sda_pin = sda_pin;
  this->scl_pin = scl_pin;
  this->clock = clock;
}

void Display::setup()
{
  i2c_init(i2c1, 400000);
  gpio_set_function(14, GPIO_FUNC_I2C);
  gpio_set_function(15, GPIO_FUNC_I2C);
  gpio_pull_up(2);
  gpio_pull_up(3);

  this->disp.external_vcc = false;
  ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
  ssd1306_clear(&disp);
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
  ssd1306_clear(&disp);
  ssd1306_show(&disp);
}