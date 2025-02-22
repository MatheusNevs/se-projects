#include "api.h"

char global_response[MAX_RESPONSE_SIZE] = {0};

static err_t tcp_recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  printf("Entrou no tcp_recv_callback\n");
  if (p == NULL)
  {
    // Connection closed
    tcp_close(pcb);
    return ERR_OK;
  }

  if (p->len < MAX_RESPONSE_SIZE)
  {
    memcpy(global_response, p->payload, p->len);
    global_response[p->len] = '\0';
  }

  printf("Received data: %.*s\n", p->len, (char *)p->payload);

  tcp_recved(pcb, p->len);
  pbuf_free(p);
  printf("Saiu do tcp_recv_callback");
  return ERR_OK;
}

err_t make_weather_request(void *arg, struct tcp_pcb *pcb, err_t err)
{
  printf("Entrou no make_weather_request\n");
  if (err != ERR_OK)
  {
    printf("Erro na conexão: %d\n", err);
    return err;
  }

  // Set the receive callback
  tcp_recv(pcb, tcp_recv_callback);

  const char *request = "GET /v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m HTTP/1.1\r\n"
                        "Host: api.open-meteo.com\r\n"
                        "Connection: close\r\n\r\n";

  err_t err_write = tcp_write(pcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
  if (err_write != ERR_OK)
  {
    printf("Failed to write request\n");
    return err_write;
  }
  printf("Saiu do make_weather_request");
  return tcp_output(pcb);
}

void get_weather_data()
{
  printf("Entrou no get_weather_data\n");
  struct tcp_pcb *pcb = tcp_new();
  ip_addr_t remote_addr;
  ip4addr_aton("3.72.202.224", &remote_addr); // IP for api.open-meteo.com

  if (tcp_connect(pcb, &remote_addr, 80, make_weather_request) != ERR_OK)
  {
    printf("Failed to connect\n");
    tcp_close(pcb);
  }
  printf("Saiu do get_weather_data");
}