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

  // setup_display();
  // draw_daily_forecast(ssd, &frame_area, "10", "february", "2025", "24", "25");

  setup_wifi(ssid, pass);
  // struct http_response_t response = get_request("api.open-meteo.com", "/v1/forecast?latitude=15.47&longitude=47.56&daily=temperature_2m_max,temperature_2m_min");
  // printf("printando buffer e index de fora da funcao");
  // printf(*response.buffer, "\n");
  // printf((char *)(*response.index), "\n");
  // struct WeatherData parsedData = process_json_response(*response.buffer, strlen(*response.buffer));

  // for (int i = 0; i < 7; i++)
  // {
  //   printf("Day %s - Max: %s°C, Min: %s°C\n", parsedData.time_data[i], parsedData.temp_max_data[i], parsedData.temp_min_data[i]);
  // }

  struct http_response_t response2 = get_request("api.open-meteo.com", "/v1/forecast?latitude=15.47&longitude=47.56&hourly=precipitation,relative_humidity_2m,temperature_2m&forecast_days=1&forecast_hours=12");
  printf("printando buffer e index de fora da funcao");
  printf(response2.buffer, "\n");
  printf((char *)(response2.index), "\n");
  struct WeatherData parsedData2 = process_json_response(response2.buffer, strlen(response2.buffer));

  for (int i = 0; i < 12; i++)
  {
    printf("Hour %s - Temp: %s°C, Humidity: %s%%, Precipitation: %s mm\n",
           parsedData2.time_data[i],
           parsedData2.temp_data[i],
           parsedData2.humid_data[i],
           parsedData2.precip_data[i]);
  }

  // Periodo 0 para diária, 1 para por hora
  // int period = 0, index = 0;
  while (true)
  {
    sleep_ms(1000);
  }

  return 0;
}
