#include "wifi.h"

// Buffer para armazenar a resposta JSON
char json_buffer[JSON_BUFFER_SIZE];
size_t json_index = 0;

uint32_t country = CYW43_COUNTRY_BRAZIL;
uint32_t auth = CYW43_AUTH_WPA2_MIXED_PSK;

int setup_wifi(const char *ssid,
               const char *pass)
{
  if (cyw43_arch_init_with_country(country))
  {
    return 1;
  }

  cyw43_arch_enable_sta_mode();
  netif_set_hostname(netif_default, "PicoW");

  if (cyw43_arch_wifi_connect_async(ssid, pass, auth))
  {
    return 2;
  }

  int flashrate = 1000;
  int status = CYW43_LINK_UP + 1;
  while (status >= 0 && status != CYW43_LINK_UP)
  {
    int new_status = cyw43_tcpip_link_status(
        &cyw43_state, CYW43_ITF_STA);
    if (new_status != status)
    {
      status = new_status;
      flashrate = flashrate / (status + 1);
      printf("connect status: %d %d\n",
             status, flashrate);
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(flashrate);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(flashrate);
  }
  if (status < 0)
  {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
  }
  else
  {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    printf("IP: %s\n",
           ip4addr_ntoa(netif_ip_addr4(netif_default)));
    printf("Mask: %s\n",
           ip4addr_ntoa(netif_ip_netmask4(netif_default)));
    printf("Gateway: %s\n",
           ip4addr_ntoa(netif_ip_gw4(netif_default)));
    printf("Host Name: %s\n",
           netif_get_hostname(netif_default));
  }
  return status;
}

err_t body_callback(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err)
{

  // Calcula espaço disponível
  uint16_t remaining = JSON_BUFFER_SIZE - json_index - 1;
  uint16_t to_copy = p->tot_len > remaining ? remaining : p->tot_len;

  if (to_copy > 0)
  {
    pbuf_copy_partial(p, json_buffer + json_index, to_copy, 0);
    json_index += to_copy;
  }

  if (to_copy < p->tot_len)
  {
    printf("Aviso: Buffer cheio! Dados truncados\n");
  }

  pbuf_free(p);
  return ERR_OK;
}

err_t headers_callback(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len)
{
  // Reseta o índice para nova requisição
  json_index = 0;
  printf("Headers recebidos. Tamanho do conteúdo: %d bytes\n", content_len);
  return ERR_OK;
}

void result_callback(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err)
{
  printf("\nResultado da requisição:\n");
  printf("Código HTTP: %d\n", srv_res);
  printf("Bytes recebidos: %d\n", rx_content_len);
}

struct http_response_t get_request(char *dns, char *address)
{
  printf("entrou no get_request");
  // Configuração da requisição HTTP
  httpc_connection_t settings = {
      .result_fn = result_callback,
      .headers_done_fn = headers_callback};

  // Faz a requisição
  err_t err = httpc_get_file_dns(
      dns,
      80,
      address,
      &settings,
      body_callback,
      NULL,
      NULL);

  if (err != ERR_OK)
  {
    printf("Erro ao iniciar requisição: %d\n", err);
  }

  sleep_ms(2000);

  struct http_response_t response = {
      0};

  memcpy(response.buffer, json_buffer, sizeof(json_buffer));
  response.index = json_index;

  memset(json_buffer, 0, JSON_BUFFER_SIZE);
  json_index = 0;

  return response;
}