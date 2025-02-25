# Monitoramento de Som com Alerta e Display SSD1306

Este projeto utiliza o microcontrolador Raspberry Pi Pico (BitDogLab) para monitorar níveis de som em um ambiente, com feedback visual e sonoro. O sistema usa um microfone para medir a intensidade sonora e exibe o valor em decibéis (dB) em um display SSD1306. Além disso, ele controla LEDs e um buzzer para alertar quando o som excede um limite predefinido.

## Componentes
- **Microfone** conectado ao pino ADC2 (GP28)
- **LEDs**:
  - LED Verde (GP11)
  - LED Vermelho (GP13)
- **Buzzer** conectado ao pino GP21
- **Botões**:
  - Botão A (GP5) para alternar o limite de dB
  - Botão B (GP6) para pausar ou retomar o monitoramento
- **Display SSD1306** conectado via I2C nos pinos GP14 (SDA) e GP15 (SCL)

## Funcionalidades
- **Leitura do Microfone**: O microfone é lido via ADC, e a tensão é convertida em dB.
- **Limite de dB**: O limite de dB pode ser alterado pressionando o Botão A. Quando o som excede o limite, um LED vermelho é aceso e o buzzer soa. Caso contrário, o LED verde é aceso.
- **Pausa**: Ao pressionar o Botão B, o monitoramento é pausado e o valor de dB é congelado. O status de pausa é mostrado no display.
- **Display SSD1306**: O valor de dB e o limite configurado são exibidos no display. A tela é atualizada a cada 100ms.
- **Controle do Buzzer**: O buzzer emite um som quando o som excede o limite configurado.

## Requisitos
- **Raspberry Pi Pico** (ou compatível)
- **BitDogLab** para GPIO e periféricos
- **Display SSD1306** (I2C)
- **Microfone analógico** para detecção de som
- **Bibliotecas**:
  - `ssd1306` para controle do display
  - `hardware/adc` para leitura do ADC
  - `hardware/pwm` para controle do buzzer

## Configuração do Hardware
- O microfone está conectado ao pino **GP28** (ADC2) para leitura analógica.
- O LED verde está no pino **GP11** e o LED vermelho no pino **GP13**.
- O buzzer está no pino **GP21**.
- O Botão A está no pino **GP5** e o Botão B no pino **GP6**.
- O display SSD1306 é conectado via I2C nos pinos **GP14 (SDA)** e **GP15 (SCL)**.

## Como Usar
1. **Iniciar o Sistema**: O sistema começa monitorando os níveis de som e exibindo os valores no display SSD1306.
2. **Alterar Limite de dB**: Pressione o Botão A para alterar o limite de dB. O limite vai alternar entre 5, 10, 30 e 60 dB.
3. **Pausar Monitoramento**: Pressione o Botão B para pausar o monitoramento. O valor de dB será congelado no display.
4. **Alerta de Som Alto**: Se o som ultrapassar o limite configurado, o LED vermelho será aceso e o buzzer emitirá um alerta.

## Exemplo de Saída no Display
- **dB:** Nível atual de decibéis medido.
- **A = x dB**: Limite de dB configurado.
- **B = rodando/pausado**: Indica se o sistema está pausado ou rodando.

## Código de Controle do Buzzer
O buzzer é controlado via PWM, variando a intensidade do som com base no nível de dB. Ele emite um som contínuo quando o nível de som ultrapassa o limite configurado.


[Clique aqui ⬆️ e assista o vídeo](https://youtu.be/UfRHBw8rvcU)


## Autor
Desenvolvido por [Renan Ferreira]
