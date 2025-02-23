#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/http_client.h"
#include "jsmn.h"

// Buffer para armazenar a resposta JSON
#define JSON_BUFFER_SIZE 4096
char json_buffer[JSON_BUFFER_SIZE];
size_t json_index = 0;

char ssid[] = "EDI";
char pass[] = "f1v3k1d5";
uint32_t country = CYW43_COUNTRY_BRAZIL;
uint32_t auth = CYW43_AUTH_WPA2_MIXED_PSK;

static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0)
  {
    return 0;
  }
  return -1;
}

void process_json_response()
{
  printf("Entrou no process json\n");
  printf(json_buffer, "\n");
  jsmn_parser parser;
  jsmntok_t tokens[256]; // Aumentado para acomodar mais tokens

  jsmn_init(&parser);
  int token_count = jsmn_parse(&parser, json_buffer, json_index, tokens, sizeof(tokens) / sizeof(tokens[0]));

  if (token_count < 0)
  {
    printf("Erro no parsing JSON: %d\n", token_count);
    return;
  }

  // Procura pelos campos desejados
  for (int i = 0; i < token_count; i++)
  {
    if (jsoneq(json_buffer, &tokens[i], "daily") == 0)
    {
      printf("entrou no daily\n");
      // Arrays para armazenar os dados como strings
      char time[7][11]; // YYYY-MM-DD format
      char temp_max[7][10];
      char temp_min[7][10];

      for (int j = i + 1; j < token_count; j++)
      {
        if (jsoneq(json_buffer, &tokens[j], "time") == 0)
        {
          // Armazena as datas
          int array_size = tokens[j + 1].size;
          for (int k = 0; k < array_size && k < 7; k++)
          {
            jsmntok_t *value_token = &tokens[j + 2 + k];
            int len = value_token->end - value_token->start;
            strncpy(time[k], json_buffer + value_token->start, len);
            time[k][len] = '\0';
          }
        }
        else if (jsoneq(json_buffer, &tokens[j], "temperature_2m_max") == 0)
        {
          // Armazena temperaturas máximas
          int array_size = tokens[j + 1].size;
          for (int k = 0; k < array_size && k < 7; k++)
          {
            jsmntok_t *value_token = &tokens[j + 2 + k];
            int len = value_token->end - value_token->start;
            strncpy(temp_max[k], json_buffer + value_token->start, len);
            temp_max[k][len] = '\0';
          }
        }
        else if (jsoneq(json_buffer, &tokens[j], "temperature_2m_min") == 0)
        {
          // Armazena temperaturas mínimas
          int array_size = tokens[j + 1].size;
          for (int k = 0; k < array_size && k < 7; k++)
          {
            jsmntok_t *value_token = &tokens[j + 2 + k];
            int len = value_token->end - value_token->start;
            strncpy(temp_min[k], json_buffer + value_token->start, len);
            temp_min[k][len] = '\0';
          }
        }
      }

      // Imprime os dados armazenados
      printf("\nDados armazenados:\n");
      for (int k = 0; k < 7; k++)
      {
        printf("Data %s: Max %s°C, Min %s°C\n",
               time[k], temp_max[k], temp_min[k]);
      }
      break;
    }
  }
}

// Callbacks HTTP
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
  process_json_response();
}

int setup(uint32_t country, const char *ssid,
          const char *pass, uint32_t auth,
          const char *hostname, ip_addr_t *ip,
          ip_addr_t *mask, ip_addr_t *gw)
{
  if (cyw43_arch_init_with_country(country))
  {
    return 1;
  }

  cyw43_arch_enable_sta_mode();
  if (hostname != NULL)
  {
    netif_set_hostname(netif_default, hostname);
  }

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
    if (ip != NULL)
    {
      netif_set_ipaddr(netif_default, ip);
    }
    if (mask != NULL)
    {
      netif_set_netmask(netif_default, mask);
    }
    if (gw != NULL)
    {
      netif_set_gw(netif_default, gw);
    }
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

int main()
{
  stdio_init_all();

  // Configuração inicial
  setup(country, ssid, pass, auth, "MyPicoW", NULL, NULL, NULL);

  // Configuração da requisição HTTP
  httpc_connection_t settings = {
      .result_fn = result_callback,
      .headers_done_fn = headers_callback};

  // Faz a requisição
  err_t err = httpc_get_file_dns(
      "api.open-meteo.com",
      80,
      "/v1/forecast?latitude=15.47&longitude=47.56&daily=temperature_2m_max,temperature_2m_min",
      &settings,
      body_callback,
      NULL,
      NULL);

  if (err != ERR_OK)
  {
    printf("Erro ao iniciar requisição: %d\n", err);
  }

  while (true)
  {
    sleep_ms(1000);
  }
}