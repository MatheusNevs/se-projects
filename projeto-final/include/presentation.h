#ifndef PRESENTATION_INTERFACE_H
#define PRESENTATION_INTERFACE_H

#include "ssd1306.h"
#include <stdio.h>

void draw_daily_forecast(uint8_t *ssd, struct render_area *frame_area,
                         char *day, char *month, char *year,
                         char *minTemperature, char *maxTemperature);

void draw_hourly_forecast(uint8_t *ssd, struct render_area *frame_area,
                          char *day, char *month, char *year,
                          char *precipitation, char *temperature, char *humidity);

#endif // PRESENTATION_INTERFACE_H