/*-----------------------------------------
----  Programa de automação de Bombas  ----  
----                                   ----  
----         Desenvolvido Por:         ----
----      Vinicius Moreira Souza       ----
----                                   ----
-------------------------------------------*/

//INSTALAÇÃO DE BIBLIOTECAS-----------------------------------------------
#include <WiFi.h>                      //Biblioteca para conectar ao Wifi
#include <IOXhop_FirebaseESP32.h>      //Biblioteca para se comunicar com Firebase
#include <ArduinoJson.h>               //Biblioteca que permite o ESP32 se comunicar com Firebase no formato JSON

// DEFININDO CONFIGURAÇÃO DO SENSOR ULTRASSÔNICO -------------------------
int pino_trigger = 2;              // Define o Pino 2 (GPIO 2 ou D2) que será conectada a saída TRIGGER do sensor ultrassônico
int pino_echo = 4;                 // Define o Pino 4 (GPIO 4 ou D4) que será conectada a saída ECHO do sensor ultrassônico
// As saídas Vcc e GND devem ser conectadas à fonte de tensão (obs: o gnd da fonte deve estar conectado ao GND deste ESP32)

//DEFININDO CONFIGURAÇÃO DE NÍVEL ---------------------------------------
int sensor_nivel =0;              // Variável dos valores de nível
int profundidade = 12;            // Defina aqui a profunDidade do Tanque armazenamento de água utilizado (400cm é o limite do sensor testado na simulação)
int distancia_min = 2;             // Distancia mínima entre a água e o sensor para evitar a queima (altura máxima)
int range_max = profundidade*58;   // Variável intermediária para calculo correto do sensor
int range_min = distancia_min*58;  // Variável intermediária para calculo correto do sensor

//DEFININDO PARÂMETROS DO WIFI-------------------------------------------
#define WIFI_SSID "JESUS CRISTO_2"                                           //Definindo Nome da Rede WIFI
#define WIFI_PASSWORD "12345687"                                             //Definindo Senha para logar no WIFI
#define FIREBASE_HOST "https://controlebombas-default-rtdb.firebaseio.com/"  //Link do Firebase
#define FIREBASE_AUTH "1K8XgXXhD1yKnWTfwCRF9LggkFhxtuUNuZLxzj6T"             // Autenticação do Firebase

//DEFININDO CONFIGURAÇÕES DO FIREBASE -----------------------------------
bool status_modo = 0;                  //Variável de coleta da chave modo no FireBase
bool status_b1 = 0;                    //Variável de coleta da chave bomba1 no FireBase
bool status_b2 = 0;                    //Variável de coleta da chave bomba2 no FireBase
int status_nivel = 0;                  //Variável de coleta da chave nivel no Firebase

//DEFININDO VARIÁVEIS DE ENDEREÇO PARA TAREFAS EM NÚCLEOS DIFERENTES------
TaskHandle_t loopcore0;               // Variável de endereço utilizada para configuração das tarefas para os núcleos individuais
TaskHandle_t loopcore1;               // Variável de endereço utilizada para configuração das tarefas para os núcleos individuais

void setup() {

  //INICIANDO CONFIGURAÇÃO DO SENSOR DE NÍVEL -----------------------------------
  Serial.begin(9600);              //Inicia o Monitor Serial em 9600 Baud
  pinMode(pino_trigger, OUTPUT);    //Define o Pino Trigger como saída de sinal
  pinMode(pino_echo,INPUT);         //Define o Pino Echo como entrada de sinal


  //INICIANDO CONEXÃO WIFI------------------------------------------------------
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //inicia conexão com SSID  e senha
  Serial.println("Conectando ao wifi"); //Imprime no monitor Serial
  while(WiFi.status() !=WL_CONNECTED){
    Serial.println("Aguardando Conexão");//Imprime no monitor Serial
    delay(300);
  }
  Serial.println("WIFI CONECTADO");     //Imprime no monitor Serial

  // INICIANDO CONEXÃO COM FIREBASE -------------------------------------------
  Serial.println("CONECTANDO AO FIREBASE");     //Imprime no monitor Serial
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //Inicia conexão com dados de autenticação
  Serial.println("FIREBASE CONECTADO");


  //DEFININDO NÚCLEO PARA TAREFA DE MONITORAMENTO DE NÍVEL
  xTaskCreatePinnedToCore(
    nucleo_leitura,// Função da tarefa
    "loopcore0",     // Nome da tarefa
    10000,           // Tamanho da pilha da tarefa (em palavras)
    NULL,            // Parâmetro passado para a tarefa
    1,               // Prioridade da tarefa
    &loopcore0,      // Identificador da tarefa
    0                // Núcleo 0
  );

  //DEFININDO NÚCLEO PARA TAREFA DE COMUNICAÇÃO 
  xTaskCreatePinnedToCore(
    nucleo_envio,// Função da tarefa
    "loopcore1",     // Nome da tarefa
    10000,           // Tamanho da pilha da tarefa (em palavras)
    NULL,            // Parâmetro passado para a tarefa
    1,               // Prioridade da tarefa
    &loopcore1,      // Identificador da tarefa
    1                // Núcleo 1
  );
}

void nucleo_leitura(void *parameter){
  while(true){
    leitura_nivel(); // Chamando Função de Leitura de nível com sensor ultrassônico
    delay(1000); // Tempo para evitar erro no processador
  }
}

void nucleo_envio(void *parameter){
  while(true){
    envio_firebase();  // Chamando Função de envio do valor de nível para o banco de dados
    delay(500); // tempo para evitar erro no processador
  }
}

//LOOP Inativo 
void loop() {
}

//FUNÇÕES UTILIZADAS

//Nucleo 0
void leitura_nivel(){                              // Função de Leitura de nível com sensor ultrassônico
      digitalWrite(pino_trigger, HIGH);           //Valor alto na entrada TRIG
      delayMicroseconds(10);                     //Intervalor de 10ms
      digitalWrite(pino_trigger, LOW);            //Valor baixo na entrada TRIG

      sensor_nivel = pulseIn(pino_echo, HIGH);                   //Coleta o valor de distancia enviado pelo sensor através da saída ECHO e atribui à variável sensor_nivel (função PulseIn mede o tamanho do pulso )
      sensor_nivel = map(sensor_nivel, range_min, range_max , 100, 0);  //Adapta o range do sensor para o range de 100 a 0

      //Impressão dos Valores no Monitor Serial para monitoramento do código (ESP32 precisa estar conectado ao computador para realizar a leitura)
      Serial.print("Nivel: ");
      Serial.print(sensor_nivel);
      Serial.println("%");
}


//Nucleo 1
void envio_firebase(){            //Função de envio do valor de nível para o banco de dados
  Firebase.setInt("/nivel",sensor_nivel); //Grava o valor na Chave "nivel" do banco de dados
}
  



