#ifndef API_INTERFACE_H
#define API_INTERFACE_H

#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"

#define MAX_RESPONSE_SIZE 4096

static err_t tcp_recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

err_t make_weather_request(void *arg, struct tcp_pcb *pcb, err_t err);

void get_weather_data();

#endif // API_INTERFACE_H