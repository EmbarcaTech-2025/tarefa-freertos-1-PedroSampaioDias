#ifndef TAREFAS_GPIO_H
#define TAREFAS_GPIO_H

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t manipulador_tarefa_led;
extern TaskHandle_t manipulador_tarefa_buzzer;

void tarefa_led(void *parametros);
void tarefa_buzzer(void *parametros);
void tarefa_botoes(void *parametros);

#endif
