#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "tarefas_gpio.h"
#include "definicoes_pinos.h"

TaskHandle_t manipulador_tarefa_led = NULL;
TaskHandle_t manipulador_tarefa_buzzer = NULL;

void tarefa_led(void *parametros) {
    gpio_init(PINO_LED_VERMELHO);
    gpio_set_dir(PINO_LED_VERMELHO, GPIO_OUT);
    gpio_put(PINO_LED_VERMELHO, 0);

    gpio_init(PINO_LED_VERDE);
    gpio_set_dir(PINO_LED_VERDE, GPIO_OUT);
    gpio_put(PINO_LED_VERDE, 0);

    gpio_init(PINO_LED_AZUL);
    gpio_set_dir(PINO_LED_AZUL, GPIO_OUT);
    gpio_put(PINO_LED_AZUL, 0);

    static uint cor = 0;

    for (;;) {
        static uint pinos[] = {PINO_LED_AZUL, PINO_LED_VERDE, PINO_LED_VERMELHO};
        static uint total = sizeof(pinos) / sizeof(pinos[0]);

        gpio_put(pinos[cor], 0);
        cor = (cor + 1) % total;
        gpio_put(pinos[cor], 1);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void tarefa_buzzer(void *parametros) {
    for (;;) {
        pwm_set_gpio_level(PINO_BUZZER, 4095);
        vTaskDelay(pdMS_TO_TICKS(100));
        pwm_set_gpio_level(PINO_BUZZER, 0);
        vTaskDelay(pdMS_TO_TICKS(900));
    }
}

void tarefa_botoes(void *parametros) {
    gpio_init(PINO_BOTAO_A);
    gpio_set_dir(PINO_BOTAO_A, GPIO_IN);
    gpio_pull_up(PINO_BOTAO_A);

    gpio_init(PINO_BOTAO_B);
    gpio_set_dir(PINO_BOTAO_B, GPIO_IN);
    gpio_pull_up(PINO_BOTAO_B);

    TickType_t debounceTicks = pdMS_TO_TICKS(DEBOUNCE_MS);

    bool lastReadingA    = false, stableStateA    = false;
    bool lastReadingB    = false, stableStateB    = false;
    TickType_t lastTimeA = 0, lastTimeB           = 0;

    for (;;) {
        TickType_t now = xTaskGetTickCount();

        bool readingA = !gpio_get(PINO_BOTAO_A);
        if (readingA != lastReadingA) {
            lastTimeA = now;
        }
        if ((now - lastTimeA) > debounceTicks && readingA != stableStateA) {
            stableStateA = readingA;
            if (stableStateA) {
                static bool suspenso_led = false;
                suspenso_led = !suspenso_led;
                if (suspenso_led) {
                    vTaskSuspend(manipulador_tarefa_led);
                } else {
                    vTaskResume(manipulador_tarefa_led);
                }
            }
        }
        lastReadingA = readingA;

        bool readingB = !gpio_get(PINO_BOTAO_B);
        if (readingB != lastReadingB) {
            lastTimeB = now;
        }
        if ((now - lastTimeB) > debounceTicks && readingB != stableStateB) {
            stableStateB = readingB;
            if (stableStateB) {
                static bool suspenso_buzzer = false;
                suspenso_buzzer = !suspenso_buzzer;
                if (suspenso_buzzer) {
                    vTaskSuspend(manipulador_tarefa_buzzer);
                } else {
                    vTaskResume(manipulador_tarefa_buzzer);
                }
            }
        }
        lastReadingB = readingB;

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}