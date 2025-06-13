#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "definicoes_pinos.h"
#include "buzzer_pwm.h"

void inicializar_buzzer(void) {
    gpio_set_function(PINO_BUZZER, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(PINO_BUZZER);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (FREQUENCIA_BUZZER * 4096));
    pwm_init(slice, &config, true);
    pwm_set_gpio_level(PINO_BUZZER, 0);
}
