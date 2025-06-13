#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "display_oled.h"
#include "definicoes_pinos.h"
#include "tarefas_gpio.h"

TaskHandle_t manipulador_tarefa_oled = NULL;
ssd1306_t display_oled;

void inicializar_display_oled(void) {
    i2c_init(PORTA_I2C, FREQUENCIA_I2C);
    gpio_set_function(PINO_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PINO_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_I2C_SDA);
    gpio_pull_up(PINO_I2C_SCL);
    display_oled.external_vcc = false;
    ssd1306_init(&display_oled, LARGURA_OLED, ALTURA_OLED, ENDERECO_I2C_OLED, PORTA_I2C);
    ssd1306_clear(&display_oled);
    ssd1306_show(&display_oled);
}

void tarefa_display_oled(void *parametros) {
    char status_led[25];
    char status_buzzer[25];

    for (;;) {
        if (manipulador_tarefa_led && manipulador_tarefa_buzzer) {
            eTaskState estado_led = eTaskGetState(manipulador_tarefa_led);
            eTaskState estado_buzzer = eTaskGetState(manipulador_tarefa_buzzer);

            ssd1306_clear(&display_oled);
            sprintf(status_led, "LED: %s", (estado_led == eSuspended) ? "Suspensa" : "Rodando ");
            sprintf(status_buzzer, "Buzzer: %s", (estado_buzzer == eSuspended) ? "Suspenso" : "Rodando");

            ssd1306_draw_string(&display_oled, 0, 20, 1, status_led);
            ssd1306_draw_string(&display_oled, 0, 40, 1, status_buzzer);
            ssd1306_show(&display_oled);
        }

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
