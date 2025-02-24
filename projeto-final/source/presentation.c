#include "presentation.h"

void draw_daily_forecast(uint8_t *ssd, struct render_area *frame_area,
                         char *day, char *month, char *year,
                         char *minTemperature, char *maxTemperature)
{
  memset(ssd, 0, ssd1306_buffer_length);
  printf("draw_daily_forecast");

  int y = 0;

  char formatted_date[32];
  const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  int month_num = atoi(month) - 1; // Convert month string to number (0-11)
  sprintf(formatted_date, "%s %s, %s", months[month_num], day, year);

  char *text1[] = {
      "Daily Forecast",
      formatted_date,
  };

  printf(formatted_date, "\n");
  printf(year, "\n");

  // Draw first text block
  for (uint i = 0; i < count_of(text1); i++)
  {
    ssd1306_draw_string(ssd, 0, y, text1[i]);
    y += 8;
  }
  render_on_display(ssd, frame_area);

  // Add horizontal line
  ssd1306_draw_line(ssd, 0, y, ssd1306_width - 1, y, true);
  render_on_display(ssd, frame_area);

  y += 8;

  // Draw second text block
  char max_temp[32], min_temp[32];
  sprintf(max_temp, "Maximum: %s C", maxTemperature);
  sprintf(min_temp, "Minimum: %s C", minTemperature);
  char *text2[] = {
      max_temp,
      min_temp,
  };

  printf(max_temp, "\n");
  printf(min_temp, "\n");

  for (uint i = 0; i < count_of(text2); i++)
  {
    ssd1306_draw_string(ssd, 0, y, text2[i]);
    y += 8;
  }
  render_on_display(ssd, frame_area);
}

void draw_hourly_forecast(uint8_t *ssd, struct render_area *frame_area,
                          char *hour, char *minute,
                          char *precipitation, char *temperature, char *humidity)
{
  printf("draw_hourly_forecast");
  memset(ssd, 0, ssd1306_buffer_length);
  int y = 0;

  char formatted_date[32];
  sprintf(formatted_date, "    %sh %sm", hour, minute);
  char *text1[] = {
      "Hourly Forecast",
      formatted_date,
  };

  printf(formatted_date, "\n");

  // Draw first text block
  for (uint i = 0; i < count_of(text1); i++)
  {
    ssd1306_draw_string(ssd, 0, y, text1[i]);
    y += 8;
  }
  render_on_display(ssd, frame_area);

  // Add horizontal line
  ssd1306_draw_line(ssd, 0, y, ssd1306_width - 1, y, true);
  render_on_display(ssd, frame_area);

  y += 8;

  // Draw second text block
  char temp[32], precip[32], hum[32];
  sprintf(temp, "Temp: %s C", temperature);
  sprintf(precip, "Precip: %s%%", precipitation);
  sprintf(hum, "Humidity: %s%%", humidity);
  char *text2[] = {
      temp,
      precip,
      hum};

  printf(temp, "\n");
  printf(precip, "\n");
  printf(hum, "\n");

  for (uint i = 0; i < count_of(text2); i++)
  {
    ssd1306_draw_string(ssd, 0, y, text2[i]);
    y += 8;
  }
  render_on_display(ssd, frame_area);
}
