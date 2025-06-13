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

#### 1. O que acontece se todas as tarefas tiverem a mesma prioridade?

Se todas as tarefas forem configuradas com a mesma prioridade no FreeRTOS, o escalonador utilizará o mecanismo de **time slicing**, ou seja, as tarefas irão compartilhar o tempo de CPU de forma igualitária, alternando a execução a cada tick do sistema. Porém, essa alternância só acontece se as tarefas fizerem chamadas que liberem a CPU, como `vTaskDelay()`, `vTaskDelayUntil()` ou esperas por eventos como semáforos. Se alguma tarefa com prioridade igual entrar em um loop contínuo sem bloqueios, ela pode monopolizar a CPU, impedindo que as outras tarefas executem, o que pode causar problemas de starvation e travamento de funcionalidades importantes.

#### 2. Qual tarefa consome mais tempo da CPU?

No projeto desenvolvido, a tarefa que mais consome tempo de CPU é a **tarefa de leitura dos botões**. Isso acontece porque ela utiliza um método de polling com verificações a cada 10 milissegundos para garantir uma resposta rápida aos eventos de botão. Mesmo utilizando `vTaskDelay` ao final de cada ciclo, sua alta frequência de execução faz com que ela exija mais atenção do processador em comparação com tarefas que operam com intervalos maiores, como o controle do LED, do buzzer ou da atualização do display OLED.

#### 3. Quais seriam os riscos de usar polling sem prioridades?

O uso de polling sem uma definição clara de prioridades traz diversos riscos ao sistema. Primeiramente, pode ocorrer a **inanição de tarefas menos frequentes**, pois o processador pode ficar ocupado constantemente com tarefas de verificação rápida, deixando outras tarefas importantes sem tempo de execução. Além disso, existe o risco de **resposta lenta a eventos críticos**, já que o sistema pode não conseguir alternar de forma eficiente entre as tarefas quando todas competem no mesmo nível de prioridade. Outro problema é o **desperdício de recursos da CPU**, já que loops de polling executados sem delays consomem processamento mesmo quando não há eventos a serem tratados, o que aumenta o consumo de energia e reduz a eficiência do sistema. Para evitar esses problemas, no projeto foi configurada uma prioridade mais alta para a tarefa de botões, além da utilização de delays mínimos para permitir a preempção pelo escalonador.

### ⚠️ Riscos e soluções implementadas
**Desafios encontrados**:  
1. **Debounce inconsistente**:  
   - Solução: Implementação de máquina de estados com timer de 50ms (`DEBOUNCE_MS`)  
2. **Conflito de recursos**:  
   - Solução: Uso de semáforos virtuais via `vTaskSuspend()`/`vTaskResume()`  
3. **Atualização do OLED**:  
   - Solução: Buffer dedicado e atualização periódica (250ms)
