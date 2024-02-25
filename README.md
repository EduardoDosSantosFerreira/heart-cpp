# Projeto Arduino de Monitor de Batimentos Cardíacos e Detecção de Riscos

![lux](https://github.com/EduardoDosSantosFerreira/System-Heart/blob/main/System_Heart.png)

O sistema consiste em analisar os dados da frequência cardíaca de uma pessoa em um certo intervalo de tempo e, após isso, retornar o status dos batimentos medidos, que serão divididos em quatro categorias:

- **Status: Normal** (de 1Hz a 1.2Hz): Quando a média dos batimentos cardíacos está normal.
- **Status: Bom** (entre 1.2Hz a 1.5Hz): Quando a média dos batimentos cardíacos está boa.
- **Status: Alerta** (entre 1.5Hz e 2.2Hz): Quando a média dos batimentos cardíacos está alta.
- **Status: Perigo** (2.2Hz ou mais): Quando a média dos batimentos cardíacos está muito alta.

## Componentes

- 1 Servo motor
- 1 Sensor ultrassônico
- 1 Sensor PIR
- 1 Osciloscópio
- 1 Gerador de funções
- 2 LCDs 16x2
- 1 LED Vermelho

Este projeto utiliza componentes eletrônicos e sensores para medir a frequência cardíaca, analisar os dados coletados e classificar os batimentos cardíacos em diferentes níveis de risco. A utilização de um sistema como esse pode auxiliar na detecção precoce de problemas cardíacos e na monitoração da saúde cardiovascular.
