#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "bh1750_light_sensor.h"
#include "ssd1306.h"

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1
#define I2C_PORT_DISP i2c1
#define I2C_SDA_DISP 14
#define I2C_SCL_DISP 15
#define endereco 0x3C
#define BUZZER_PIN 10
#define LUX_MIN 50
#define LUX_MAX 700

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


    // Inicializa sensor de luz BH1750
    bh1750_power_on(I2C_PORT);

    char str_lux[16];
    bool cor = true;

    while (true) {
        uint16_t lux = bh1750_read_measurement(I2C_PORT);
        sprintf(str_lux, "%d Lux", lux);
        printf("Lux = %d\n", lux);

        if (lux < LUX_MIN) {
            buzzer_beep(400, 300);
        } else if (lux > LUX_MAX) {
            buzzer_beep(1500, 200);
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
