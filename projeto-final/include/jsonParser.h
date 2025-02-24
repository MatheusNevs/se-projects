#ifndef JSON_PARSER_INTERFACE_H
#define JSON_PARSER_INTERFACE_H

#include "jsmn.h"
#include <string.h>
#include <stdio.h>

struct WeatherData
{
  char daily_data[7][11];
  char hourly_data[12][6];
  char temp_min_data[7][7];
  char temp_max_data[7][7];
  char precip_data[12][7];
  char humid_data[12][7];
  char temp_data[12][7];
};

struct WeatherData process_json_response(const char *json_buffer, size_t buffer_size);

#endif // JSON_PARSER_INTERFACE_H