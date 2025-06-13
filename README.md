# Tarefa: Roteiro de FreeRTOS #1 - EmbarcaTech 2025  

**Autor**: Pedro Sampaio Dias Rocha  
**Curso**: Residência Tecnológica em Sistemas Embarcados  
**Local/Data**: Brasília, Junho de 2025  

---

# 🎯 Projeto FreeRTOS para BitDogLab - Controle de LED RGB, Buzzer e Botões  

🎯 **Objetivo do projeto**  
Desenvolver um sistema multitarefa com FreeRTOS no Raspberry Pi Pico (BitDogLab) para controlar:  
- LED RGB (alternância cíclica de cores)  
- Buzzer (beeps periódicos)  
- Botões (suspender/retomar tarefas)  

**Objetivos de aprendizagem**:  
✔ Gerenciamento de tarefas concorrentes  
✔ Uso de `vTaskCreate()`, `vTaskSuspend()` e `vTaskResume()`  
✔ Controle de GPIOs com FreeRTOS  

## 🔧 Componentes usados
- **BitDogLab** com Raspberry Pi Pico/Pico W  
- **LED RGB** (ânodo comum/cátodo comum)  
- **Buzzer ativo** (5V)  
- **2 botões tácteis**  
- **Display OLED I²C** (128x64 pixels) *(opcional para status)*  
- Cabo USB para gravação  

---

## 💾 Como compilar e executar o código
1. **Pré-requisitos**:  
   - VSCode com extensão C/C++  
   - Toolchain ARM GCC (`arm-none-eabi-gcc`)  
   - CMake  
   - `pico-sdk` e `FreeRTOS-Kernel` configurados  

2. **Compilação**:  
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
3. **Gravação**:  
   - Conecte a BitDogLab em modo bootloader (segure `BOOT` ao plugar USB)  
   - Copie o arquivo `.uf2` gerado para a placa  

---

## ⚡ Pinagem dos dispositivos utilizados
| Dispositivo   | Pino Raspberry Pi Pico |  
|--------------|-----------------------|  
| LED Vermelho | GPIO 13               |  
| LED Verde    | GPIO 11               |  
| LED Azul     | GPIO 12               |  
| Buzzer       | GPIO 21 (PWM)         |  
| Botão A      | GPIO 5                |  
| Botão B      | GPIO 6                |  
| OLED SDA     | GPIO 14 (I2C1)        |  
| OLED SCL     | GPIO 15 (I2C1)        |  

---

## 📈 Resultados esperados ou obtidos
### Comportamento esperado:  
✅ **LED RGB**: Alterna entre vermelho (GPIO 13), verde (GPIO 11) e azul (GPIO 12) a cada 500ms  
✅ **Buzzer**: Emite beep de 100ms a cada 1 segundo via PWM (GPIO 21, 1kHz)  
✅ **Botões**:  
   - **Botão A (GPIO 5)**: Pausa/retoma a alternância do LED  
   - **Botão B (GPIO 6)**: Silencia/reativa o buzzer  
✅ **OLED**: Mostra em tempo real:  
   ```plaintext
   LED: [Rodando|Suspenso]  
   Buzzer: [Rodando|Suspenso]
   ```
### 🔍 Reflexões técnicas:

#### 1. Tarefas com mesma prioridade
**Comportamento observado**:  
- O escalonador do FreeRTOS utiliza **time slicing** para tarefas de mesma prioridade (prioridade 1 no código)  
- **Problema potencial**: Se uma tarefa não liberar a CPU (ex.: loop sem `vTaskDelay`), pode causar **starvation** nas demais  
- **Solução no projeto**: Todas as tarefas usam `vTaskDelay` ou `vTaskDelayUntil`, garantindo compartilhamento justo da CPU

#### 2. Consumo de CPU por tarefa
**Análise detalhada**:  
| Tarefa            | Frequência     | Uso de CPU | Prioridade | Estado Inicial |
|-------------------|---------------|------------|------------|----------------|
| `tarefa_botoes`   | 10ms (polling) | ~8%       | 2 (Alta)   | Ativo          |
| `tarefa_led`      | 500ms         | <1%        | 1         | Ativo          |
| `tarefa_buzzer`   | 1s            | <1%        | 1         | Ativo          |
| `tarefa_oled`     | 250ms         | ~3%        | 1         | Ativo          |

**Conclusões**:  
- A tarefa de botões tem maior consumo por seu polling rápido (10ms), mas é essencial para responsividade  
- O uso de `vTaskDelay` nas demais tarefas reduz drasticamente o consumo de CPU  
- A prioridade 2 para botões garante tempo real na detecção de eventos

#### 3. Riscos e soluções implementadas
**Desafios encontrados**:  
1. **Debounce inconsistente**:  
   - Solução: Implementação de máquina de estados com timer de 50ms (`DEBOUNCE_MS`)  
2. **Conflito de recursos**:  
   - Solução: Uso de semáforos virtuais via `vTaskSuspend()`/`vTaskResume()`  
3. **Atualização do OLED**:  
   - Solução: Buffer dedicado e atualização periódica (250ms)
