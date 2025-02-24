#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "include/ssd1306.h"
#include "hardware/i2c.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"
#include "wifi.h"
#include "presentation.h"
#include "jsonParser.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

struct render_area frame_area;
uint8_t ssd[ssd1306_buffer_length];

char ssid[] = "EDI";
char pass[] = "f1v3k1d5";

#define BUTTON_SWITCH 5 // Button to switch between daily and hourly forecast
#define BUTTON_NEXT 6   // Button to advance day/hour

void setup_display()
{
  // Inicialização do i2c
  i2c_init(i2c1, ssd1306_i2c_clock * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  // Processo de inicialização completo do OLED SSD1306
  ssd1306_init();

  // Preparar área de renderização
  frame_area = (struct render_area){
      .start_column = 0,
      .end_column = ssd1306_width - 1,
      .start_page = 0,
      .end_page = ssd1306_n_pages - 1};

  calculate_render_area_buffer_length(&frame_area);

  // zera o display inteiro
  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);
}

int main()
{
  stdio_init_all();

  // Initialize display and buttons
  setup_display();
  gpio_init(BUTTON_SWITCH);
  gpio_init(BUTTON_NEXT);
  gpio_set_dir(BUTTON_SWITCH, GPIO_IN);
  gpio_set_dir(BUTTON_NEXT, GPIO_IN);
  gpio_pull_up(BUTTON_SWITCH);
  gpio_pull_up(BUTTON_NEXT);

  // Setup WiFi and get initial daily forecast
  setup_wifi(ssid, pass);

  // Get initial daily forecast
  struct http_response_t daily_response = get_request("api.open-meteo.com",
                                                      "/v1/forecast?latitude=15.47&longitude=47.56&daily=temperature_2m_max,temperature_2m_min");
  struct WeatherData daily_data = process_json_response(daily_response.buffer, strlen(daily_response.buffer));

  // Get initial hourly forecast
  struct http_response_t hourly_response = get_request("api.open-meteo.com",
                                                       "/v1/forecast?latitude=15.47&longitude=47.56&hourly=precipitation,relative_humidity_2m,temperature_2m&forecast_days=1&forecast_hours=12");
  struct WeatherData hourly_data = process_json_response(hourly_response.buffer, strlen(hourly_response.buffer));

  bool is_daily = true; // true for daily forecast, false for hourly
  int current_index = 0;
  bool last_switch_state = true;
  bool last_next_state = true;

  // Initial display
  char day[3], month[10], year[5];
  sscanf(daily_data.daily_data[current_index], "%4s-%2s-%2s", year, month, day);
  draw_daily_forecast(ssd, &frame_area,
                      day,
                      month,
                      year,
                      daily_data.temp_min_data[current_index],
                      daily_data.temp_max_data[current_index]);

  while (true)
  {
    bool current_switch_state = gpio_get(BUTTON_SWITCH);
    bool current_next_state = gpio_get(BUTTON_NEXT);

    // Switch button pressed (mode change)
    if (current_switch_state == false && last_switch_state == true)
    {
      is_daily = !is_daily;
      current_index = 0;

      if (is_daily)
      {
        char day[3], month[10], year[5];
        sscanf(daily_data.daily_data[current_index], "%4s-%2s-%2s", year, month, day);
        draw_daily_forecast(ssd, &frame_area,
                            day,
                            month,
                            year,
                            daily_data.temp_min_data[current_index],
                            daily_data.temp_max_data[current_index]);
      }
      else
      {
        char hour[3], minute[3];
        sscanf(hourly_data.hourly_data[current_index], "%2s:%2s", hour, minute);
        draw_hourly_forecast(ssd, &frame_area,
                             hour,
                             minute,
                             hourly_data.temp_data[current_index],
                             hourly_data.humid_data[current_index],
                             hourly_data.precip_data[current_index]);
      }
    }

    // Next button pressed (advance index)
    if (current_next_state == false && last_next_state == true)
    {
      if (is_daily)
      {
        current_index = (current_index + 1) % 7;
        char day[3], month[10], year[5];
        sscanf(daily_data.daily_data[current_index], "%4s-%2s-%2s", year, month, day);
        draw_daily_forecast(ssd, &frame_area,
                            day,
                            month,
                            year,
                            daily_data.temp_max_data[current_index],
                            daily_data.temp_min_data[current_index]);
      }
      else
      {
        current_index = (current_index + 1) % 12;
        char hour[3], minute[3];
        sscanf(hourly_data.hourly_data[current_index], "%2s:%2s", hour, minute);
        draw_hourly_forecast(ssd, &frame_area,
                             hour, minute,
                             hourly_data.temp_data[current_index],
                             hourly_data.humid_data[current_index],
                             hourly_data.precip_data[current_index]);
      }
    }

    last_switch_state = current_switch_state;
    last_next_state = current_next_state;
    sleep_ms(100); // Debounce delay
  }

  return 0;
}
