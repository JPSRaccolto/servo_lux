#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "bh1750_light_sensor.h"
#include "ssd1306.h"
#include "hardware/pwm.h"

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1
#define I2C_PORT_DISP i2c1
#define I2C_SDA_DISP 14
#define I2C_SCL_DISP 15
#define endereco 0x3C
#define BUZZER_PIN 10
#define LUX_MIN 70
#define LUX_MAX 700
#define SERVO_PIN 28
#define PULSO_MIN 1000
#define PULSO_MAX 2000
#define PULSO_OFF 1500

ssd1306_t ssd;

void buzzer_init() {
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
}

void buzzer_tone(uint freq, uint duration_ms) {
    if (freq == 0) {
        sleep_ms(duration_ms);
        return;
    }
    uint delay = 1000000 / (2 * freq);
    uint cycles = (duration_ms * 1000) / (2 * delay);

    for (uint i = 0; i < cycles; i++) {
        gpio_put(BUZZER_PIN, 1);
        sleep_us(delay);
        gpio_put(BUZZER_PIN, 0);
        sleep_us(delay);
    }
}

void buzzer_beep(uint freq, uint duration_ms) {
    buzzer_tone(freq, duration_ms);
    sleep_ms(50);
}

int main() {
    stdio_init_all();
    buzzer_init();

    // Inicializa I2C para o BH1750
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa I2C para o Display
    i2c_init(I2C_PORT_DISP, 400 * 1000);
    gpio_set_function(I2C_SDA_DISP, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_DISP, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_DISP);
    gpio_pull_up(I2C_SCL_DISP);

    // Inicializa display SSD1306
    ssd1306_init(&ssd, 128, 64, false, endereco, I2C_PORT_DISP);
    ssd1306_config(&ssd);   
    ssd1306_fill(&ssd, false);

    // Inicializa e configura o PWM 
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 19999);
    pwm_init(slice_num, &config, true);


    // Inicializa sensor de luz BH1750
    bh1750_power_on(I2C_PORT);

    char str_lux[16];
    bool cor = true;

    while (true) {
        uint16_t lux = bh1750_read_measurement(I2C_PORT);
        sprintf(str_lux, "%d Lux", lux);
        printf("Lux = %d\n", lux);

        if (lux < LUX_MIN) { // Caso a luminosidade esteja abaixo do limite mínimo, o servo é acionado em um sentido de rotação
            buzzer_beep(400, 300);
            pwm_set_gpio_level(SERVO_PIN, PULSO_MIN);
        } else if (lux > LUX_MAX) { // Se a luminosidade exceder o limite máximo, o servomotor é acionado em sentido oposto
            buzzer_beep(1500, 200);
            pwm_set_gpio_level(SERVO_PIN, PULSO_MAX);
        } else if (lux > LUX_MIN && lux < LUX_MAX){ //Se a luminosidade estiver na faixa intermediária, o servo fica parado
            pwm_set_gpio_level(SERVO_PIN, PULSO_OFF);
        }

        ssd1306_fill(&ssd, false); // limpa a tela
        ssd1306_draw_string(&ssd, " EMBARCATECH ", 16, 0);  
        ssd1306_hline(&ssd, 0, 127, 10, true); // linha horizontal abaixo do cabeçalho
        ssd1306_draw_string(&ssd, " Luminosidade ", 20, 20);
        ssd1306_draw_string(&ssd, str_lux, 40, 40);
        ssd1306_send_data(&ssd);


        sleep_ms(500);
    }
}
