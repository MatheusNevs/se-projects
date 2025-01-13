#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

const uint BUTTON_A_PIN = 5; // Botão A no GPIO 5
const uint BUTTON_B_PIN = 6; // Botão B no GPIO 6

const uint BLUE_LED_PIN = 12;  // LED azul no GPIO 12
const uint RED_LED_PIN = 13;   // LED vermelho no GPIO 13
const uint GREEN_LED_PIN = 11; // LED verde no GPIO 11

#define BUZZER_PIN 21

void beep(uint pin, int frequency, int duration_ms)
{
    int delay_us = 1000000 / (2 * frequency);      // Tempo para meio ciclo (em microssegundos)
    int cycles = (frequency * duration_ms) / 1000; // Número total de ciclos

    for (int i = 0; i < cycles; i++)
    {
        gpio_put(pin, 1);   // Liga o buzzer
        sleep_us(delay_us); // Espera meio ciclo
        gpio_put(pin, 0);   // Desliga o buzzer
        sleep_us(delay_us); // Espera outro meio ciclo
    }
}

void setup()
{
    stdio_init_all();

    // Configuração dos botões
    gpio_init(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_pull_up(BUTTON_B_PIN);

    // Configuração dos LEDs
    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    // Inicialização dos LEDs em estado desligado
    gpio_put(RED_LED_PIN, 0);
    gpio_put(GREEN_LED_PIN, 0);
    gpio_put(BLUE_LED_PIN, 0);

    // Configuração do GPIO para o buzzer como saída
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
}

void readPassword(int *password)
{
    int index = 0;
    while (index < 4)
    {
        // Espera até que um botão seja pressionado
        while (gpio_get(BUTTON_A_PIN) && gpio_get(BUTTON_B_PIN))
        {
            tight_loop_contents(); // Evita consumo desnecessário de CPU
        }
        password[index] = gpio_get(BUTTON_A_PIN) ? 1 : 0; // Botão A: 1, Botão B: 0
        index++;
        sleep_ms(500); // Debounce e tempo para próximo dígito
    }
}

int checkPassword(const int *password, const int *passwordTry)
{
    for (int i = 0; i < 4; i++)
    {
        if (password[i] != passwordTry[i])
        {
            return 0; // Senha incorreta
        }
    }
    return 1; // Senha correta
}

int main()
{
    setup();

    int password[4];    // Senha original
    int passwordTry[4]; // Tentativa de senha

    // Entrada da senha original
    gpio_put(BLUE_LED_PIN, 1); // LED azul aceso para indicar entrada de senha
    readPassword(password);
    gpio_put(BLUE_LED_PIN, 0); // LED azul apagado
    beep(BUZZER_PIN, 440, 500);

    while (true)
    {

        // Entrada da tentativa de senha
        gpio_put(GREEN_LED_PIN, 1); // LED verde aceso para indicar entrada da tentativa
        gpio_put(RED_LED_PIN, 1);   // LED vermelho aceso também
        readPassword(passwordTry);

        if (checkPassword(password, passwordTry))
        {
            // Senha correta
            gpio_put(RED_LED_PIN, 0); // Apaga o LED vermelho
            beep(BUZZER_PIN, 800, 200);
            sleep_ms(250);
            beep(BUZZER_PIN, 800, 200);

            sleep_ms(2000);             // Espera 30 segundos
            gpio_put(GREEN_LED_PIN, 0); // Apaga o LED verde
            continue;
        }

        // Senha incorreta
        gpio_put(GREEN_LED_PIN, 0); // LED vermelho aceso
        beep(BUZZER_PIN, 150, 500);
        sleep_ms(2000);           // Indicação de erro por 2 segundos
        gpio_put(RED_LED_PIN, 0); // LED vermelho apagado
    }

    return 0;
}
