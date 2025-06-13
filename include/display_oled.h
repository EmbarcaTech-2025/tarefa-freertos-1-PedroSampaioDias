#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include "FreeRTOS.h"
#include "task.h"
#include "ssd1306.h"

extern TaskHandle_t manipulador_tarefa_oled;
extern ssd1306_t display_oled;

void inicializar_display_oled(void);
void tarefa_display_oled(void *parametros);

#endif
