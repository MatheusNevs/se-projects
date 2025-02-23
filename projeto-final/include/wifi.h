#ifndef WIFI_INTERFACE_H
#define WIFI_INTERFACE_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/http_client.h"

#define JSON_BUFFER_SIZE 4096

struct http_response_t
{
  char buffer[JSON_BUFFER_SIZE];
  int index;
};

int setup_wifi(const char *ssid,
               const char *pass);

struct http_response_t get_request(char *dns, char *address);

#endif // WIFI_INTERFACE_H