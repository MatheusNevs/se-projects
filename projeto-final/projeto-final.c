#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "include/ssd1306.h"
#include "hardware/i2c.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"
#include "api.h"
#include "presentation.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

#define WIFI_SSID "Wifi"     // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASS "Password" // Substitua pela senha da sua rede Wi-Fi

struct render_area frame_area;
uint8_t ssd[ssd1306_buffer_length];

void setup_display()
{
    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização
    frame_area = (struct render_area){
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1};

    calculate_render_area_buffer_length(&frame_area);

    // zera o display inteiro
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
}

void setup_wifi()
{
    if (cyw43_arch_init())
    {
        printf("Erro ao inicializar o Wi-Fi\n");
        return;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000))
    {
        printf("Falha ao conectar ao Wi-Fi\n");
        return;
    }
    else
    {
        printf("Connected.\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    printf("Wi-Fi conectado!\n");
}

int main()
{
    stdio_init_all();

    setup_display();
    draw_daily_forecast(ssd, &frame_area, "10", "february", "2025", "24", "25");

    // setup_wifi();
    // get_weather_data();

    // Periodo 0 para diária, 1 para por hora
    // int period = 0, index = 0;
    while (true)
    {
        // cyw43_arch_poll();
        sleep_ms(1000);
    }

    return 0;
}
