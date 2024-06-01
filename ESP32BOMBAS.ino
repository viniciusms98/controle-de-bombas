
/*-----------------------------------------
----  Programa de automação de Bombas  ----  
----                                   ----  
----         Desenvolvido Por:         ----
----      Vinicius Moreira Souza       ----
----                                   ----
-------------------------------------------*/

// Essa automação é referente ao ESP32 conectado às bombas e ao comando manual
// O código do outro ESP32 fará o monitoramento do nível da água e enviará para o Firebase Real DataBase

//INCLUSÃO DAS BIBLIOTECAS
#include <Arduino.h>
#include <WiFi.h>                             //Biblioteca para conectar ao Wifi
#include <IOXhop_FirebaseESP32.h>             //Biblioteca para se comunicar com Firebase
#include <ArduinoJson.h>                      //Biblioteca que permite o ESP32 se comunicar com Firebase no formato JSON3
#include <Wire.h>                             //Biblioteca para comunicação I2C
#include <LiquidCrystal_I2C.h>                //Biblioteca do Display 20x4 I2C

//Parametros do Wifi
#define WIFI_SSID "JESUS CRISTO_2"                                                   //Definindo Nome da Rede WIFI
#define WIFI_PASSWORD "12345687"                                                     //Definindo Senha para logar no WIFI
#define FIREBASE_HOST "https://controlebombas-default-rtdb.firebaseio.com/"          //Link do Firebase
#define FIREBASE_AUTH "1K8XgXXhD1yKnWTfwCRF9LggkFhxtuUNuZLxzj6T"                     // Autenticação do Firebase

//PARÂMETROS DO DISPLAY
#define SDA_PIN 21                            //Define o Pino para SDA do Display (Dados)
#define SCL_PIN 22                            //Define o Pino para SCL do Display (Clock)
LiquidCrystal_I2C lcd(0x27, 20, 4);           //Criação do Objeto LCD
/*O primeiro parâmetro é o endereço, o segundo o número de colunas e o terceiro o número de linhas*/

/*Pinagem do Display
GND: No GND do ESP 32
VCC: No Vcc 5V do ESP32
SDA: No D21 do ESP32 
SCL: No D22 do ESP32*/


int pin_b1=12;                    //Pino de Ativação do Relé da Bomba 1
int pin_b2=14;                    //Pino de Ativação do Relé da Bomba 2

int pin_modo=15;                  //Pino de comando de modo
int pin_manual_b1=16;             //Pino de comando manual da bomba 1
int pin_manual_b2=17;             //Pino de comando manual da bomba 2

bool status_modo = true;          // Variável intermediária de status_modo (True - Automático / False - Manual)
bool status_modo_man = true;
bool status_b1 = false;           //Variável intermediária de status da bomba 1
bool status_b2 = false;           //Variável intermediária de status da bomba 2
int sensor_nivel = 0;             //Variável usada para registrar o nível


TaskHandle_t loopcore0;
TaskHandle_t loopcore1;


void setup(){
  Wire.begin(SDA_PIN, SCL_PIN);             // Inicializa a comunicação I2C com os pinos especificados
  lcd.backlight();                          // Luz de Fundo do LCD
  lcd.begin(20, 4);                         //Iniciando o LCD
  lcd.clear();                              // Limpando o LCD

  pinMode(pin_b1, OUTPUT);                  //Definindo Pino de Saída da Bomba 1
  pinMode(pin_b2, OUTPUT);                  //Definindo Pino de Saída da Bomba 2
  pinMode(pin_modo, INPUT_PULLUP);          //Definindo Pino do modo Manual/Automático
  pinMode(pin_manual_b1,INPUT_PULLUP);      //Definindo Pino do Modo manual da Bomba 1
  pinMode(pin_manual_b2, INPUT_PULLUP);     //Definindo Pino do Modo manual da Bomba 2
  
  digitalWrite(pin_b1,HIGH);                //Mantendo pino do relé desligado
  digitalWrite(pin_b2,HIGH);                //Mantendo pino do relé desligado

  //BOAS VINDAS
   lcd.setCursor(5,1);
   String palavra = "Bem-Vindo!";
    for (int i = 0; i < palavra.length(); i++) {
    lcd.print(palavra[i]); // Imprime o caractere atual
    delay(200); // Aguarda um curto período de tempo para a próxima letra
  }
   delay(2000);
   lcd.clear();

  //Fazendo Login do Wifi

    lcd.setCursor(2,1);
    lcd.print("Conectando Wifi");
    delay(1000);

   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
  while(WiFi.status() !=WL_CONNECTED){
    lcd.setCursor(4, 1);
    lcd.print("Conectando...      ");
    delay(150);
    lcd.clear();
    delay(150);
  }


  lcd.clear();
  lcd.setCursor(3, 1);
  lcd.print("Wifi Conectado");
  delay(2000);
  lcd.clear();

  lcd.setCursor(3, 1);
  lcd.print("Conectando ao");
  lcd.setCursor(5, 2);
  lcd.print("Firebase");

  // Fazendo conexão com o firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //Enviando status iniciais dos modos no firebase
  Firebase.setBool("/modo",status_modo);
  Firebase.setBool("/bomba1",status_b1);
  Firebase.setBool("/bomba2",status_b2);

  //Obtendo primeiro valor de nível
  int sensor_nivel = Firebase.getInt("/nivel");

  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print("CONECTADO!");
  delay(1000);
  lcd.clear(); 

  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("CONFIGURACOES");
  lcd.setCursor(6, 2);
  lcd.print("INICIAIS");
  delay(1000);
  lcd.clear();
   
  //Iniciando bombas 1 e 2 como desligadas e enviando código para o Firebase 
  nivel();
  bomba1_off();
  bomba2_off();
  verificacao_modo();
  

  xTaskCreatePinnedToCore(
    funcore0,       // Função da tarefa
    "loopcore0",     // Nome da tarefa
    10000,           // Tamanho da pilha da tarefa (em palavras)
    NULL,            // Parâmetro passado para a tarefa
    1,               // Prioridade da tarefa
    &loopcore0,      // Identificador da tarefa
    0                // Núcleo 0
  );

  // Crie a tarefa para o Core 1
  xTaskCreatePinnedToCore(
    funcore1,       // Função da tarefa
    "loopcore1",     // Nome da tarefa
    10000,           // Tamanho da pilha da tarefa (em palavras)
    NULL,            // Parâmetro passado para a tarefa
    1,               // Prioridade da tarefa
    &loopcore1,      // Identificador da tarefa
    1                // Núcleo 1
  );

}

void verificacao_modo(){

  //Os if's abaixo verificam o status atual da variável status_modo no firebase e imprimem na tela o estado atual  

    status_modo = Firebase.getBool("/modo");

    if (status_modo == false){
        lcd.setCursor(0, 1);                       //Seta o lugar do cursor  
        lcd.print("Modo:Manual     ");                     //Imprime no LCD
      }
    if (status_modo == true){
        lcd.setCursor(0, 1);                       //Seta o lugar do cursor
        lcd.print("Modo:Automatico");                     //Imprime no LCD
    }
  

}

void modo_automatico(){
    verificacao_modo();                                    // Verifica se o modo de operação continua automático
    nivel();

  while (sensor_nivel< nvl_max && status_modo == true){  //Verifica se o nível é menor que 100 e a variável status_modo está true, ou seja, no modo automático
    nivel();                                            //Imprime o nível
    verificacao_modo();                                 // Verifica se o modo de operação continua automático

        if (sensor_nivel>50 && status_modo == true){    //Esse If garante que a bomba 2 não ficará ligada acima de 50% no modo automático
              if(status_b2 == true){bomba2_off();}      //Verifica o estado atual da bomba1
              }
    while (sensor_nivel <50 && status_modo == true){    //Se o nível descer a menos de 50, esse While entra em ação
        
        if(status_b1 == false){bomba1_on();}             //Com nível baixo de 50% a bomba 1 é ligada
        if (sensor_nivel>20 && status_modo == true) {          //Garante que enquanto não atingir 20% a bomba 2 não ligará
        if(status_b2 == true){bomba2_off();}}
        
        verificacao_modo();                              // Verifica se o modo de operação continua automático
        nivel();

        while (sensor_nivel < nvl_max && status_modo == true){//Esse While rege o funcionamento das bombas desde o momento que o nível desce de 50% até ele se reestabelecer ao nivel máximo desejado
              verificacao_modo();                  
              if(status_b1 == false){bomba1_on();}
              nivel();                          

          if (sensor_nivel<20 && status_modo == true ){       //Se o nível descer abaixo de 20% a bomba 2 é ativada
              if(status_b2 ==false){bomba2_on();}
          }
          if (sensor_nivel>50 && status_modo == true && status_b2 == true){       //Desliga a bomba 2 quando o nível passa de 50% dentro desse While
              if(status_b2 == true){bomba2_off();}
            }
        }
     }
  }

  if (sensor_nivel >= nvl_max){                         //Garante o desligamento da bomba 1 ao chegar ao nível máximo desejado
      if(status_b1 == true){bomba1_off();}
      if(status_b2 == true){bomba2_off();}                                          
      }

}

void modo_manual(){
    while(status_modo == false){
      //Obtém as informações de status no Firebase
      verificacao_modo();
      nivel();
      bool new_status_b1 = Firebase.getBool("/bomba1");

      bool new_status_b2 = Firebase.getBool("/bomba2");


      if (status_b1 != new_status_b1){            // Verifica se houve alteração no valor da chave da b1, se houve, faz a alteração.

            status_b1 = new_status_b1;

            if (status_b1 == true){
                bomba1_on();
            }
            if (status_b1 == false){
                bomba1_off();
            }
            }
 
      if (status_b2 != new_status_b2){          // Verifica se houve alteração no valor da chave da b2, se houve, faz a alteração.
    
            status_b2 = new_status_b2;

            if (status_b2 == true){
                bomba2_on();
            }

            if (status_b2 == false){
                bomba2_off();
            }

      }
    }     
}

void nivel(){


      sensor_nivel = Firebase.getInt("/nivel");


      //sensor_nivel = Firebase.getInt("/nivel");   //Obtendo o valor de nível do Firebase e incluindo na variável
      //Impressão no LCD
      lcd.setCursor(0, 0);                       //setando posição do cursor de escrita no LCD
      lcd.print("Nivel:");                         //Imprime no LCD
      lcd.print(sensor_nivel);                   //Imprimindo a variável
      lcd.print("%  ");                          //Imprime no LCD
     

      
}

//Função de desligamento da Bomba 1
void bomba1_off(){
      
      status_b1= false;                            //Define a variável status_b1 como false
      digitalWrite(pin_b1,HIGH);                    //Define a saída da bomba um como LOW, desligando a bomba 1
      lcd.setCursor(0, 2);                        //Seta o local do cursor
      lcd.print("Bomba 1: DESLIGADA");                         //Imprime o valor no LCD
      Firebase.setBool("/bomba1",status_b1);       //Envia o novo status de status_b1 para o firebase
      nivel();                                     //Escreve o nível na tela
}

//Função de Ligar a bomba 1
void bomba1_on(){

      status_b1= true;                              //Define a variável status_b1 como true
      digitalWrite(pin_b1,LOW);                    //Define a saída da bomba um como HIGH, ligando a bomba 1
      lcd.setCursor(0, 2);                         //Seta o local do cursor
      lcd.print("Bomba 1: LIGADA   ");                          //Imprime o valor no LCD
      Firebase.setBool("/bomba1",status_b1);        //Envia o novo status de status_b1 para o firebase
      nivel();                                      //Escreve o nível na tela
}
 
//Função de desligamento da Bomba 2
void bomba2_off() {
      status_b2= false;                             //Define a variável status_b1 como false
      digitalWrite(pin_b2,HIGH);                     //Define a saída da bomba um como LOW, desligando a bomba 2
      lcd.setCursor(0, 3);                         //Seta o local do cursor
      lcd.print("Bomba 2: DESLIGADA");                          //Imprime o valor no LCD
      Firebase.setBool("/bomba2",status_b2);        //Envia o novo status de status_b2 para o firebase
      nivel();                                      //Escreve o nível na tela
}

//Função de Ligar a bomba 2
void bomba2_on(){

      status_b2= true;                               //Define a variável status_b1 como false
      digitalWrite(pin_b2,LOW);                     //Define a saída da bomba um como HIGH, ligando a bomba 2
      lcd.setCursor(0, 3);                          //Seta o local do cursor
      lcd.print("Bomba 2: LIGADA   ");                           //Imprime o valor no LCD
      Firebase.setBool("/bomba2",status_b2);         //Envia o novo status de status_b2 para o firebase
      nivel();                                       //Escreve o nível na tela
}


//Função Loop principal
void funcore0(void *parameter){
  while(true){
   verificacao_modo();                                 //Verificação do Modo Automático/Manual

  if(status_modo == false){                            //Se a variável status_modo for false,ativa a função de modo Manual
    modo_manual();
  }
  if(status_modo == true){                            //Se a variável status_modo for true, ativa a função de modo Automático
    modo_automatico();
  }
  }
}

//função de monitoramento do botão
void funcore1(void *parameter){
  while(true){
    //MODO

    // Esse IF Verifica se o botão de modo automático/manual foi pressionado (pressionado ele ganha o valor LOW)
    if (digitalRead(pin_modo) == LOW){
          status_modo_man = !status_modo;              //Se o botão foi pressionado, o valor da variável "status_modo" é invertido.
          Firebase.setBool("/modo",status_modo_man);   // Enviando a variável com novo valor para o Firebase
     }


     //BOMBA 1 E 2 - MODO MANUAL

        if (digitalRead (pin_manual_b1) == LOW && status_modo == false){                     //Verifica se houve pressionamento do botão de acionamento/desligamento manual da bomba 1
        bool new_status1 = !status_b1;               //Se o botão foi pressionado, ele muda o estado da variável
        Firebase.setBool("/bomba1",new_status1);
        }
    
    if (digitalRead (pin_manual_b2) == LOW  && status_modo == false){      //Verifica se houve pressionamento do botão de acionamento manual da bomba 2
        
        bool new_status2 = !status_b2;           //Se o botão foi pressionado, ele muda o estado da variável
        Firebase.setBool("/bomba2",new_status2);
      }
  }
}

void loop(){
}
