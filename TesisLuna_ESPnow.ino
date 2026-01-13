#include "CustomEspNow.h"
#include "src/DeltaTime/DeltaTime.h"
#include "src/TouchSensor/TouchSensor.h"



//Parametros Modificables
//===================================================
char* escultura = "TRISTE";                                           //"FELIZ" || "TRISTE"
const float triggerToMimitos = 5.0;                                   //Tiempo de interaccion para Mimito || INTERACION -> MIMITOS
const int minimo_Sensores_Activados_To_Mimitos = 2;                   //Minimo de sensores activados para contar "Mimito" || INTERACION -> MIMITOS

//Pines usados
//===================================================
#define SENSOR_10_PIN 4  //Primer trio
#define SENSOR_11_PIN 16
#define SENSOR_12_PIN 17
#define SENSOR_20_PIN 18  //Segundo trio
#define SENSOR_21_PIN 19
#define SENSOR_22_PIN 21
#define SENSOR_30_PIN 34  //Tercer trio
#define SENSOR_31_PIN 35
#define SENSOR_32_PIN 32
#define SENSOR_40_PIN 33  //Cuarto trio
#define SENSOR_41_PIN 25
#define SENSOR_42_PIN 26

#define LEDS_BOCA 13  //--> Luces
#define LEDS_ENTORNO 12


//Maquina de estados
//===================================================
#define ESPERANDO 0
#define HAY_INTERACCION 1
#define HAY_MIMITOS 2
int state = 0;
float cronometroToMimitos;

//Librerias
//===================================================
DeltaTime deltaTime;

//Debug
//===================================================
int contadorInteraccion;
int contadorMimitos;



void setup() {
  Serial.begin(115200);
  EspNowInit();
}


void loop() {
  // EspNowMessage msg;
  // strcpy(msg.a, "THIS IS A CHAR");
  // msg.b = random(1, 20);
  // msg.c = 1.2;
  // msg.d = false;

  EspNowSendExample();

  delay(2000);
}









