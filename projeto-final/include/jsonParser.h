#ifndef JSON_PARSER_INTERFACE_H
#define JSON_PARSER_INTERFACE_H

#include "jsmn.h"
#include <string.h>
#include <stdio.h>

struct WeatherData
{
  char time_data[7][12];
  char temp_min_data[7][7];
  char temp_max_data[7][7];
  char precip_data[7][12];
  char humid_data[7][12];
  char temp_data[7][12];
};

struct WeatherData process_json_response(const char *json_buffer, size_t buffer_size);

#endif // JSON_PARSER_INTERFACE_H