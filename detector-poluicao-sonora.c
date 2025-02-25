#include <stdio.h>  // Biblioteca padrão para entrada e saída de dados (printf, scanf, etc.).
#include "pico/stdlib.h"  // Biblioteca padrão do Raspberry Pi Pico (GPIO, UART, timers, etc.).
#include "hardware/adc.h"  // Biblioteca para uso do conversor analógico-digital (ADC).
#include "ssd1306.h"  // Biblioteca para controle do display OLED SSD1306 via I2C.
#include "hardware/pwm.h"  // Biblioteca para controle de sinais PWM (modulação por largura de pulso).
#include "hardware/clocks.h"  // Biblioteca para manipulação dos clocks do RP2040.

// Definições de pinos e valores
#define ADC_PIN 28        // Pino do microfone no BitDogLab (GP28 - ADC2)
#define VREF 3.3          // Tensão de referência da Raspberry Pi Pico
#define ADC_MAX 4095.0    // Resolução do ADC (12 bits)
#define V_REF_MIC 1.0     // Tensão de referência do microfone (ajuste conforme necessário)
#define EPSILON 0.001     // Pequeno valor para evitar log(0)

#define LED_VERDE 11      // LED verde no GPIO 11
#define LED_VERMELHO 13   // LED vermelho no GPIO 13

#define BUZZER_PIN 21     // Pino do buzzer
#define BUZZER_FREQ_HZ 3200 // Frequência do buzzer (Hz)
#define HIGH 4096         // Valor máximo para PWM
#define LOW 0             // Valor mínimo para PWM

#define BOTAO_A_PIN 5     // Botão A conectado ao GPIO 5
#define BOTAO_B_PIN 6     // Botão B conectado ao GPIO 6

#define I2C_ADDR 0x3C     // Endereço I2C do display SSD1306
#define I2C_SDA 14        // Pino SDA
#define I2C_SCL 15        // Pino SCL

ssd1306_t disp;  // Declara a variável para o display

// Função para iniciar o controle do buzzer via PWM
void inicia_buzzer_pwm() {
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);  // Define o pino do buzzer para PWM
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);  // Obtém o número do slice para o pino do buzzer
    
    pwm_config cfg = pwm_get_default_config();  // Obtém a configuração padrão para PWM
    pwm_config_set_wrap(&cfg, HIGH - 1);  // Define o valor máximo de contagem do PWM
    pwm_config_set_clkdiv(&cfg, clock_get_hz(clk_sys) / (BUZZER_FREQ_HZ * HIGH));  // Define o divisor de clock para a frequência do buzzer
    
    pwm_init(slice_num, &cfg, true);  // Inicializa o PWM
    pwm_set_gpio_level(BUZZER_PIN, LOW);  // Garante que o buzzer comece desligado
}

// Função para ligar o buzzer
void buzzer_ligar() {
    pwm_set_gpio_level(BUZZER_PIN, HIGH / 2);  // Liga o buzzer com intensidade média
}

// Função para desligar o buzzer
void buzzer_desligar() {
    pwm_set_gpio_level(BUZZER_PIN, LOW);  // Desliga o buzzer
}

// Função para calcular o nível de dB a partir da tensão medida
float calcular_db(float tensao) {
    if (tensao < EPSILON) {
        return 0.0;  // Se a tensão for muito baixa, considera-se 0 dB
    }
    return 20 * log10(tensao / V_REF_MIC);  // Calcula o dB com base na fórmula de decibéis
}

int main() {
    stdio_init_all();  // Inicializa a comunicação serial
    sleep_ms(1000);    // Espera 1 segundo para estabilizar
    adc_init();        // Inicializa o ADC

    // Configura os LEDs
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    // Configura os botões
    gpio_init(BOTAO_A_PIN);
    gpio_set_dir(BOTAO_A_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_A_PIN);  // Ativa o pull-up interno no botão A

    gpio_init(BOTAO_B_PIN);
    gpio_set_dir(BOTAO_B_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_B_PIN);  // Ativa o pull-up interno no botão B

    // Configura o pino de ADC para o microfone
    adc_gpio_init(ADC_PIN);
    adc_select_input(2);

    // Inicializa o buzzer
    inicia_buzzer_pwm();

    // Limites de dB para controle de som
    int limites_db[] = {5, 10, 30, 60};
    int indice_limite = 0;
    int limite_db = limites_db[indice_limite];  // Define o limite inicial de dB

    bool pausa = false;  // Variável de controle de pausa
    float db_pausado = 0.0;  // Armazena o valor de dB quando em pausa

    // Configura a comunicação I2C para o display SSD1306
    i2c_init(i2c1, 400 * 1000);  // Inicializa o I2C com 400 kHz de frequência
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);  // Configura o pino SDA para I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);  // Configura o pino SCL para I2C
    gpio_pull_up(I2C_SDA);  // Ativa o pull-up no pino SDA
    gpio_pull_up(I2C_SCL);  // Ativa o pull-up no pino SCL
    
    ssd1306_init(&disp, 128, 64, I2C_ADDR, i2c1);  // Inicializa o display SSD1306 com I2C

    while (1) {
        // Verifica se o botão A foi pressionado para alternar o limite de dB
        if (!gpio_get(BOTAO_A_PIN)) {
            indice_limite = (indice_limite + 1) % 4;  // Alterna o índice do limite de dB
            limite_db = limites_db[indice_limite];  // Atualiza o limite de dB
            printf("Limite em dB escolhido: %d\n", limite_db);
            sleep_ms(300);  // Espera para evitar múltiplos cliques
        }

        // Verifica se o botão B foi pressionado para pausar ou retomar
        if (!gpio_get(BOTAO_B_PIN)) {
            pausa = !pausa;  // Alterna o estado de pausa
            printf("Pausa: %s\n", pausa ? "Sim" : "Não");
            if (pausa) {
                gpio_put(LED_VERDE, 0);  // Desliga o LED verde
                gpio_put(LED_VERMELHO, 0);  // Desliga o LED vermelho
                buzzer_desligar();  // Desliga o buzzer
                // Salva o valor de dB no momento em que a pausa é acionada
                db_pausado = calcular_db(adc_read() * (VREF / ADC_MAX));
            } else {
                printf("Sistema retomado.\n");
            }
            sleep_ms(300);  // Espera para evitar múltiplos cliques
        }

        // Se não estiver em pausa, faz a leitura do som
        if (!pausa) {
            uint16_t adc_valor = adc_read();  // Lê o valor do ADC
            float tensao = adc_valor * (VREF / ADC_MAX);  // Converte o valor do ADC para tensão
            float db = calcular_db(tensao);  // Calcula o valor de dB

            if (db < 0.0) {
                db = 0.0;  // Garante que o valor de dB não seja negativo
            }

            // Limpar a tela antes de desenhar as novas informações
            ssd1306_clear(&disp);

            // Atualiza a tela com o nível de dB
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "dB:%.1f", db);
            ssd1306_draw_string(&disp, 1, 0, 3, buffer);

            // Informações do limite de dB
            snprintf(buffer, sizeof(buffer), "A = %d dB", limite_db);
            ssd1306_draw_string(&disp, 1, 25, 2, buffer);

            // Estado de pausa
            snprintf(buffer, sizeof(buffer), "B = rodando");
            ssd1306_draw_string(&disp, 1, 45, 2, buffer);

            ssd1306_show(&disp);  // Atualiza o display

            // Controle de LEDs e buzzer com base no nível de dB
            if (db < limite_db - 1) {
                printf("Som Baixo: %.2f dB\n", db);
                gpio_put(LED_VERDE, 1);  // Liga o LED verde
                gpio_put(LED_VERMELHO, 0);  // Desliga o LED vermelho
                buzzer_desligar();  // Desliga o buzzer
            } 
            else if (db >= limite_db) { 
                printf("Som Alto: %.2f dB\nATENÇÃO!!! Volume excede ao permitido!!!\n", db);
                gpio_put(LED_VERDE, 0);  // Desliga o LED verde
                gpio_put(LED_VERMELHO, 1);  // Liga o LED vermelho
                buzzer_ligar();  // Liga o buzzer
            } 
            else {
                printf("Som Normal: %.2f dB\n", db);
                gpio_put(LED_VERDE, 1);  // Liga o LED verde
                gpio_put(LED_VERMELHO, 0);  // Desliga o LED vermelho
                buzzer_desligar();  // Desliga o buzzer
            }
        } else {
            // Se estiver em pausa, exibe a mensagem de pausa com o valor de dB congelado
            ssd1306_clear(&disp);

            // Exibe a mensagem "Pausado"
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "B = pausado");
            ssd1306_draw_string(&disp, 1, 45, 2, buffer);

            // Exibe o valor de dB congelado
            snprintf(buffer, sizeof(buffer), "dB:%.1f", db_pausado);
            ssd1306_draw_string(&disp, 1, 0, 3, buffer);

            // Informações do limite
            snprintf(buffer, sizeof(buffer), "A = %d dB", limite_db);
            ssd1306_draw_string(&disp, 1, 25, 2, buffer);

            ssd1306_show(&disp);  // Atualiza o display
        }

        sleep_ms(100);  // Aguarda 100ms antes de fazer a próxima leitura
    }

    return 0;
}
