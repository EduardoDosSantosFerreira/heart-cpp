// Projeto - Monitor de batimentos cardíacos e detecção de riscos - Eduardo S Ferreira

/*
   O sistema consiste em analisar os dados da frequencia cardiaca
   de um pessoa em um certo intervalo de tempo.
   E, apos isso, retornar o status dos batimentos medidos que serao
   divididos em quatro status;
   
   Status: Normal (de 1hz a 1.2 hz)
   Quando a media dos batimentos cardiacos esta normal.
   Status: Bom (entre 1.2hz a 1.5hz)
   Quando a media dos batimentos cardiacos esta boa.
   Status: Alerta (entre 1.5 hz e 2.2 hz)
   Quando a media dos batimentos cardiacos esta alta.
   Status: Perigo (2.2 hz ou mais)
   Quando a media dos batimentos cardiacos esta muito alta
   
   **Componentes:
   
   1 Servo motor;
   1 Sensor ultrassônico;
   1 Sensor PIR;
   1 Osciloscópio;
   1 Gerador de funções;
   2 Lcds 16x2;
   1 Led Vermelho;
*/

#include <LiquidCrystal.h>
#include <Servo.h>

#define trig 5
#define echo 4

LiquidCrystal lcd1(6,7,10,11,12,13);
LiquidCrystal lcd2(8,9,10,11,12,13);
Servo motor;

// Recebera a leitura do gerador de funcoes;
int   leitura;

// Variavel usada para controle do servo motor
int   pos = 0;

/*
   Variáveis time e dist serao reponsáveis por armazenar, respectivamente,
   o tempo que a onda sonora emitida pelo sensor demorou para chegar à
   pessoa até o sensor, e a distância que este esta do sensor.
   Onde a dist é a metade da distância percorrida pela onda, que será 
   calculada usando a fórmula => delta X = v*delta T, onde v é a velocidade 
   do som no ar, à temperatura ambiente, v ~ 340,29 m/s;
*/
int   time, dist;

// Acumula a diferenca entre os tempos (em milisegundos) do ligar e apagar do led, para checagem da frequencia media dos batimentos cardiacos
float mediaDif, ciclosCounter;

// Variaveis responsaveis por armazenar, respectivamente, o tempo (em milisengundos) do acender do lad, do apagar, e para o calculo da diferenca entre esses tempos
float timeStart, timeEnd, timeDif;

// Variavel responsavel por indicar se pessoa foi detectada pelo sensor pir para ativar o sistema
bool  presenca = false;

// Variaveis responsaveis por indicar o inicio e fim de leitura do gerador de funcoes
bool  startLeitura, leituraCompleta;

// Variaveis responsaveis por setar a aparicao da dica correta no lcd uma unica vez apos a leitura ser completada
bool  dica_1, dica_2, dica_3, dica_4, ciclo;

// Variaveis que representa, respectivamente, o limite de contagem dos ciclos e o contador de ciclos
const byte counterLimit = 50;
byte  counter;

void setup()
{
  // Inicializa os displays
  Serial.begin(9600);
  lcd1.begin(16,2);
  lcd2.begin(16,2);
  
  // Inicializa as variveis globais
  varsStart();
  
  // Inicializa os pinos para os lcds e os componeste do sistema
  pinMode(2, OUTPUT);    // Servo motor
  
  // Pinos para os lcds
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(A3, OUTPUT);   // Pino para o led vermelho
  pinMode(A2, INPUT);    // Pino para gerador de funcoes
  pinMode(trig, OUTPUT); // Sensor ultrassônico
  pinMode(echo, INPUT);  // Sensor ultrassônico
  
  // Inicializa o servo motor
  motor.attach(2);
  motor.write(0);
  
  // Pino de interrupcao para o sensor pir
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), acionar, RISING);
}

void loop()
{
  if (presenca) // O sistema sera ativado assim que for detectado alguma movimento pelo sensor pir
  {
    
    lcd1.display();
    lcd2.display();
    proxySensor();
    servoMotor();
    statusCheck();
    
    /* 
      Se a distancia do dedo da pessoa ao sensor for menor 
      que 0.18 m e a leitura nao estiver completa, a funcao
      freqCardica sera chamada.
    */
    if (dist <= 180 && !leituraCompleta)
        freqCardiaca();
      
  }
  else
  {
     analogWrite(A3, 0);
     lcd1.noDisplay();
     lcd2.noDisplay();
  }
 
}
/*
  Funcao freqCardiaca(): sera executada sempre que a leitura
  nao estiver completa, e o contador nao chegar a seu limite
  maximo defido anteriormente (50 ciclos).
  
  A logica implementada na funcao segue a seguinte ideia:
  se o dado lido pelo gerador de funcoes estiver entre ]-400, 400[
  o led vermelho sera aceso, caso contraio, sera desligado.
  
  Apos acionar o led, sera armazenado o instante em que ele foi
  aceso, e, apos desligar, sera armazenado o tempo que foi des-
  ligado.
  
  Se a diferenca entre os tempos for muito pequena, significa que
  o led acendeu e apagou rapido, seguindo a logica dada pela formula
  f = 1/T. Logo, tera uma frequencia alta.
  
  E, se caso a diferenca entre os tempos for alta, a frequencia
  sera baixa.
  
  **Para uma melhor estimativa dos dados da frequencia cardiaca,
  sera feito um calculo onde o tempo total dos batimentos no ciclo
  definido no projeto (50 ciclos no loop do arduino) sera dado pelo
  periodo medio. Logo a frequencia calculada sera a frequencia media
  dos batimentos cardiacos nos 50 ciclos definidos no codigo.
*/
void freqCardiaca() 
{
  if (counter != counterLimit)
  {
    
    leitura = analogRead(A2);
    
    if (leitura > 400 || leitura < -400)
    {
        delay(20);
        analogWrite(A3, 255);
        delay(20);
        timeStart = millis();
    }
    else
    {
        delay(20);
        analogWrite(A3, 0);
        delay(20);
        timeEnd = millis();
        ciclo = true;
    }
    
    delay(130);
    
    if (ciclo)
    {
      timeDif = timeEnd - timeStart;
    
      if (timeDif < 0)
          timeDif = -timeDif;
    
      if (timeDif < 1000)
      {
          mediaDif += timeDif;
          ciclosCounter++;
      }
      ciclo = false;
    }
    
    counter++;
    
  }
  else
  {
    leituraCompleta = true;
    analogWrite(A3, 0);
    mediaDif /= ciclosCounter;
  }
  
}

void proxySensor() 
{
  // Emite um pulso sonoro de 40 kHz
  digitalWrite(trig, 1);
  delayMicroseconds(10);
  digitalWrite(trig, 0);
  
  // Retorna a duracao do pulso em microsegundos
  time = pulseIn(echo, 1);
  
  /*
    v ~ 340,29 m/s = 34029 cm/s = 34029*10^-6 cm/us
    v ~ 0.034029 cm/us
    como a distancia até o dedo da pessoa é a metade
    da distância percorrido pelo som, então d = dx/2 = v*dt/2
    d = dt*0.034029/2 => d = dt*0.0170145;
  */
  
  dist = time*0.0170145;
  
  delay(10);
  
}

void servoMotor() 
{
  if (dist <= 180 && pos <= 0) servoOpen();
  
  if (dist > 180 && pos >= 90) servoClose();
}

void servoOpen()
{
   for (pos = 0; pos <= 90; pos += 3) 
        motor.write(pos), delay(20);
}

void servoClose()
{
   for (pos = 90; pos >= 0; pos -= 3) 
        motor.write(pos), delay(20);
}

void varsStart()
{
  analogWrite(A3, 0);
  leituraCompleta = false;
  startLeitura    = false;
  dica_1 = dica_2 = dica_3 = dica_4 = ciclo = false;
  mediaDif = ciclosCounter = timeStart = timeEnd = timeDif = 0;
  counter = 0;
  lcd1.clear();
  lcd2.clear();
}

void statusCheck()
{
  if (dist > 180 && leituraCompleta)
      varsStart();
  
  else if (dist > 180) 
      exibirMsg(7);
  
  else if (dist <= 180 && counter < counterLimit && !startLeitura)
      exibirMsg(6);
    
  else if (dist <= 180 && counter >= counterLimit && leituraCompleta)
      exibirMsg(8);
}

int exibirDica()
{
  int dica;
  
  if (mediaDif < 330 && mediaDif > 280) //Normal (1 hz à 1.2 hz)
     dica = 1;
  
  else if (mediaDif <= 280 && mediaDif > 255) //Bom (1.2 hz à 1.5 hz)
     dica = 2;
    
  else if (mediaDif <= 255 && mediaDif > 206) //Alerta (1.5 hz à 2.2 hz)
     dica = 3;
    
  else if (mediaDif <= 206) //Perigo (2.2 hz ou mais)
     dica = 4;
  
  exibirMsg(dica);
  return dica;
}

void printLcd1Status()
{
  
  lcd1.setCursor(0,1);
  
  switch (exibirDica())
  {
    case 1:
      lcd1.print("Status: Normal");
    break;
    case 2:
      lcd1.print("Status: Bom");
    break;
    case 3:
      lcd1.print("Status: Alerta!");
    break;
    default:
      lcd1.print("Status: Perigo!");
    break;
  }
}

// Funcao responsavel por exibir as mensagens nos lcds;

void exibirMsg(int index)
{
  switch (index)
  {
    case 1:
      dica1();
    break;
    case 2:
      dica2();
    break;
    case 3:
      dica3();
    break;
    case 4:
      dica4();
    break;
    case 5:
      msgDados();
    break;
    case 6:
      msgAnalise();
    break;
    case 7:
      msgAproximar();
    break;
    default:
      msgReaproximar();
    break;
  }
}

void msgDados()
{
  if ((dica_1 == dica_2) && (dica_3 == dica_4))
  {
    lcd2.clear();
    lcd1.clear();
    lcd2.setCursor(0,0);
    lcd2.print("Dados analisados");
    delay(1500);
    lcd2.clear();
  }
  
  lcd1.clear();
  lcd1.setCursor(0,0);
  lcd1.print("Analisado!");
  printLcd1Status();
}

void msgAnalise()
{
  startLeitura = true;
  analogWrite(A3, 0);
  lcd2.clear();
  lcd1.clear();
  lcd1.setCursor(0,0);
  lcd1.print("Analisando...");
    
  lcd2.setCursor(0,0);
  lcd2.print("Aguarde...");
  delay(1000);
  lcd2.clear();
  lcd2.setCursor(0,1);
  lcd2.print("Coletando dados!");
  delay(1000);
  lcd2.clear();
}

void msgAproximar()
{
  lcd2.clear();
  lcd2.setCursor(0,0);
  lcd2.print("Ola! ");
  delay(1000);
  lcd2.clear();
  lcd2.setCursor(0,0);
  lcd2.print("Aproxime seu dedo do sensor!");
  delay(1000);
    
  for (int i = 0; i < 12; i++)
       lcd2.scrollDisplayLeft(), delay(106);
  delay(200);
  lcd2.clear();
}

void msgReaproximar()
{
  startLeitura = false;
  
  msgDados();
  lcd2.clear();
  lcd2.setCursor(0,1);
  lcd2.print("Para uma nova leitura, reaproxime");
  delay(500);
  for (int i = 0; i < 17; i++)
       lcd2.scrollDisplayLeft(), delay(106);
  delay(500);
  lcd2.clear();
  lcd2.setCursor(0,1);
  lcd2.print("o dedo do sensor");
  delay(1000);
  lcd2.clear();
}

// Dicas;

void dica1()
{
  if (dica_1 == false)
  {
    dica_1 = true;
    lcd2.clear();
    lcd2.setCursor(0,0);
    lcd2.print("Status: Normal!");
    delay(1000);
    lcd2.clear();
    lcd2.setCursor(0,1);
    lcd2.print("Para manter esse status, mantenha uma");
    delay(500);
    for (int i = 0; i < 21; i++)
         lcd2.scrollDisplayLeft(), delay(106);
    delay(800);
    lcd2.clear();
    lcd2.setCursor(0,1);
    lcd2.print("Alimentacao Saudavel!");
    delay(300);
    for (int i = 0; i < 6; i++)
         lcd2.scrollDisplayLeft(), delay(106);
    delay(1500);
  }
}

void dica2()
{
  if (dica_2 == false)
  {
    dica_2 = true;
    lcd2.clear();
    lcd2.setCursor(0,0);
    lcd2.print("Status: Bom!");
    delay(1000);
    lcd2.clear();
    lcd2.setCursor(0,1);
    lcd2.print("Para melhorar o status,");
    delay(700);
    for (int i = 0; i < 7; i++)
         lcd2.scrollDisplayLeft(), delay(106);
    delay(800);
    lcd2.clear();
    lcd2.setCursor(0,1);
    lcd2.print("Adote uma Alimentacao Saudavel!");
    delay(200);
    for (int i = 0; i < 14; i++)
         lcd2.scrollDisplayLeft(), delay(106);
    delay(1500);
  }
}

void dica3()
{
  if (dica_3 == false)
  {
    dica_3 = true;
    lcd2.clear();
    lcd2.setCursor(0,0);
    lcd2.print("Status: Alerta!");
    delay(1000);
    lcd2.clear();
    lcd2.setCursor(0,1);
    lcd2.print("Pode ser nervosismo ou algo grave.");
    delay(500);
    for (int i = 0; i < 19; i++)
         lcd2.scrollDisplayLeft(), delay(106);
    delay(800);
    lcd2.clear();
    lcd2.setCursor(0,1);
    lcd2.print("Recomenedamos que procure um medico!");
    delay(200);
    for (int i = 0; i < 20; i++)
         lcd2.scrollDisplayLeft(), delay(106);
    delay(1000);
  }
}

void dica4()
{
  if (dica_4 == false)
  {
    dica_4 = true;
    lcd2.clear();
    lcd2.setCursor(0,0);
    lcd2.print("Status: Ruim!");
    delay(1000);
    lcd2.clear();
    lcd2.setCursor(0,1);
    lcd2.print("Nada bom, recomendamos");
    delay(500);
    for (int i = 0; i < 6; i++)
         lcd2.scrollDisplayLeft(), delay(106);
    delay(500);
    lcd2.clear();
    lcd2.setCursor(0,1);
    lcd2.print("que procure um medico!");
    delay(300);
    for (int i = 0; i < 6; i++)
         lcd2.scrollDisplayLeft(), delay(106);
    delay(1500);
  }
}

// Se a funcao for chamada, resetara todas as varivaies e desligara o sistema
void acionar() {
  presenca = !presenca;
  varsStart();
  lcd1.noDisplay();
  lcd2.noDisplay();
  if (pos >= 90) servoClose();
}
