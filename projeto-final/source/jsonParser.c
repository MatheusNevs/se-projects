#include "jsonParser.h"

static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0)
  {
    return 0;
  }
  return -1;
}

struct WeatherData process_json_response(const char *json_buffer, size_t buffer_size)
{
  struct WeatherData data = {0};
  printf("Entrou no process json\n");
  printf("printando json_buffer de dentro do process\n");
  printf(json_buffer);
  jsmn_parser parser;
  jsmntok_t tokens[256]; // Aumentado para acomodar mais tokens

  jsmn_init(&parser);
  int token_count = jsmn_parse(&parser, json_buffer, buffer_size, tokens, sizeof(tokens) / sizeof(tokens[0]));

  if (token_count < 0)
  {
    printf("Erro no parsing JSON: %d\n", token_count);
    return data;
  }

  // Arrays separados para cada tipo de dado
  char daily_data[7][11];   // Data - formato YYYY-MM-DD
  char hourly_data[12][6];  // Hora - formato HH:MM
  char temp_min_data[7][7]; // Temperatura mínima - XX.X
  char temp_max_data[7][7]; // Temperatura máxima - XX.X
  char precip_data[12][7];  // Precipitação - XXX.X
  char humid_data[12][7];   // Umidade - XXX.X
  char temp_data[12][7];    // Temperatura atual - XX.X

  // Procura pelos campos desejados
  for (int i = 0; i < token_count; i++)
  {
    if (jsoneq(json_buffer, &tokens[i], "daily") == 0 || jsoneq(json_buffer, &tokens[i], "hourly") == 0)
    {
      printf("Entrou no daily\n");

      for (int j = i + 1; j < token_count; j++)
      {
        if (jsoneq(json_buffer, &tokens[j], "time") == 0)
        {
          if (jsoneq(json_buffer, &tokens[i], "daily") == 0)
          {
            int array_size = tokens[j + 1].size;
            for (int k = 0; k < array_size && k < 7; k++)
            {
              jsmntok_t *value_token = &tokens[j + 2 + k];
              int len = value_token->end - value_token->start;
              strncpy(daily_data[k], json_buffer + value_token->start, len);
              daily_data[k][len] = '\0';
            }
          }
          else
          {
            // For hourly format, take HH:00 from YYYY-MM-DDThh:00
            for (int k = 0; k < 12; k++)
            {
              jsmntok_t *value_token = &tokens[j + 2 + k];
              int start = value_token->start + 11; // Skip to hour position
              int len = 5;                         // Take HH:00
              strncpy(hourly_data[k], json_buffer + start, len);
              hourly_data[k][len] = '\0';
            }
          }
        }
        else if (jsoneq(json_buffer, &tokens[j], "temperature_2m_min") == 0)
        {
          int array_size = tokens[j + 1].size;
          for (int k = 0; k < array_size && k < 7; k++)
          {
            jsmntok_t *value_token = &tokens[j + 2 + k];
            int len = value_token->end - value_token->start;
            strncpy(temp_min_data[k], json_buffer + value_token->start, len);
            temp_min_data[k][len] = '\0';
          }
        }
        else if (jsoneq(json_buffer, &tokens[j], "temperature_2m_max") == 0)
        {
          int array_size = tokens[j + 1].size;
          for (int k = 0; k < array_size && k < 7; k++)
          {
            jsmntok_t *value_token = &tokens[j + 2 + k];
            int len = value_token->end - value_token->start;
            strncpy(temp_max_data[k], json_buffer + value_token->start, len);
            temp_max_data[k][len] = '\0';
          }
        }
        else if (jsoneq(json_buffer, &tokens[j], "precipitation") == 0)
        {
          int array_size = tokens[j + 1].size;
          for (int k = 0; k < array_size && k < 12; k++)
          {
            jsmntok_t *value_token = &tokens[j + 2 + k];
            int len = value_token->end - value_token->start;
            strncpy(precip_data[k], json_buffer + value_token->start, len);
            precip_data[k][len] = '\0';
          }
        }
        else if (jsoneq(json_buffer, &tokens[j], "relative_humidity_2m") == 0)
        {
          int array_size = tokens[j + 1].size;
          for (int k = 0; k < array_size && k < 12; k++)
          {
            jsmntok_t *value_token = &tokens[j + 2 + k];
            int len = value_token->end - value_token->start;
            strncpy(humid_data[k], json_buffer + value_token->start, len);
            humid_data[k][len] = '\0';
          }
        }
        else if (jsoneq(json_buffer, &tokens[j], "temperature_2m") == 0)
        {
          int array_size = tokens[j + 1].size;
          for (int k = 0; k < array_size && k < 12; k++)
          {
            jsmntok_t *value_token = &tokens[j + 2 + k];
            int len = value_token->end - value_token->start;
            strncpy(temp_data[k], json_buffer + value_token->start, len);
            temp_data[k][len] = '\0';
          }
        }
      }

      // Imprime os dados armazenados(debugging)
      // printf("\nDados armazenados:\n");
      // for (int k = 0; k < 12; k++)
      // {
      //   printf("Data %s: Temp %.1f°C, Max %.1f°C, Min %.1f°C, Precip %.1fmm, Umid %.1f%%\n",
      //          time_data[k], atof(temp_data[k]),
      //          atof(temp_max_data[k]), atof(temp_min_data[k]),
      //          atof(precip_data[k]), atof(humid_data[k]));
      // }
      break;
    }
  }
  memcpy(data.humid_data, humid_data, sizeof(humid_data));
  memcpy(data.precip_data, precip_data, sizeof(precip_data));
  memcpy(data.temp_data, temp_data, sizeof(temp_data));
  memcpy(data.temp_max_data, temp_max_data, sizeof(temp_max_data));
  memcpy(data.temp_min_data, temp_min_data, sizeof(temp_min_data));
  memcpy(data.daily_data, daily_data, sizeof(daily_data));
  memcpy(data.hourly_data, hourly_data, sizeof(hourly_data));
  return data;
}
