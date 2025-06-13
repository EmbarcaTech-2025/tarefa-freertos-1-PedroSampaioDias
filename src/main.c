#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "tarefas_gpio.h"
#include "display_oled.h"
#include "buzzer_pwm.h"
#include "definicoes_pinos.h"

int main() {
    stdio_init_all();
    inicializar_display_oled();
    inicializar_buzzer();

    if (xTaskCreate(tarefa_led, "LED", 256, NULL, 1, &manipulador_tarefa_led) != pdPASS ||
        xTaskCreate(tarefa_buzzer, "Buzzer", 256, NULL, 1, &manipulador_tarefa_buzzer) != pdPASS ||
        xTaskCreate(tarefa_botoes, "Botoes", 256, NULL, 2, NULL) != pdPASS ||
        xTaskCreate(tarefa_display_oled, "OLED", 256, NULL, 1, &manipulador_tarefa_oled) != pdPASS) {
        while (true);
    }

    vTaskStartScheduler();
    while (true);
    return 0;
}
