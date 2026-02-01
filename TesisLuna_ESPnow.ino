//Library versions are detailed in the README
#include "Arduino.h"
#include "src/CustomEspNow/CustomEspNow.h"
#include "src/AudioPlayer/AudioPlayer.h"
#include "src/Statue/Statue.h"

#include "src/DeltaTime/DeltaTime.h"
#include "src/TouchSensor/TouchSensor.h"


//Parametros Modificables
//===================================================
char* escultura = "TRISTE";                          //"FELIZ" || "TRISTE"
const float triggerToMimitos = 5.0;                  //Tiempo de interaccion para Mimito || INTERACION -> MIMITOS
const int minimo_Sensores_Activados_To_Mimitos = 1;  //Minimo de sensores activados para contar "Mimito" || INTERACION -> MIMITOS
const uint8_t TOTAL_SENSORS = 9;                     //Numero de sensores activos. Si hay menos de 9, los que faltne, no hacen pinout


//Pines usados
//===================================================
#define SENSOR_10_PIN 27 //Verificar estos pines <----------------
#define SENSOR_11_PIN 26 
#define SENSOR_12_PIN 25 

#define SENSOR_20_PIN 36  //pin SVP (Solo input)
#define SENSOR_21_PIN 39  //pin SVN (Solo input)
#define SENSOR_22_PIN 34

#define SENSOR_30_PIN 22
#define SENSOR_31_PIN 35
#define SENSOR_32_PIN 21

#define SENSOR_40_PIN 25
#define SENSOR_41_PIN 33
#define SENSOR_42_PIN 32

// Luces
#define LEDS_BOCA 13     //Se podrian unificar
#define LEDS_ENTORNO 12  //PIN PROBLEMATICO

// MP3 module
#define RXD2 16 // esp receiver (rx) --> module transciever (tx) 
#define TXD2 17 // D2 = segundo puerto serial, pero es solo nombre


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

//Objects
//===================================================
Statue statue(Statue::StatueName::happy, LEDS_BOCA);

TouchSensor sensors[9] = {
  //id 21... "2" fila y "1" columna
  TouchSensor(SENSOR_10_PIN, 10),
  TouchSensor(SENSOR_11_PIN, 11),
  TouchSensor(SENSOR_12_PIN, 12),
  TouchSensor(SENSOR_20_PIN, 20),
  TouchSensor(SENSOR_21_PIN, 21),
  TouchSensor(SENSOR_22_PIN, 22),
  TouchSensor(SENSOR_30_PIN, 30),
  TouchSensor(SENSOR_31_PIN, 31),
  TouchSensor(SENSOR_32_PIN, 32)
};


//Debug
//===================================================
int contadorInteraccion;
int contadorMimitos;



void setup() {
  Serial.begin(115200);
  Mp3ModuleInit(RXD2, TXD2);
  EspNowInit();

  //Leds
  statue.InitLeds();
  statue.TurnOnLEDs(true);

  //Sensors init
  for (int i = 0; i < TOTAL_SENSORS; i++) {
    sensors[i].Init();
    sensors[i].SetDebugMode(true);
    delay(10);
  }

  delay(1000);
  Serial.println("setup is complete");
  Serial.println("--------------------------------------------");
}


void loop() {
  //EspNowSendExample();

  //Sensors update
  for (int i = 0; i < TOTAL_SENSORS; i++) {
    sensors[i].Update(deltaTime.Get());
  }

  for (int i = 0; i < TOTAL_SENSORS; i++) {
    if (sensors[i].isTouchStable()) {
      EspNowSendExample();
      PlaySound(statue.TRACK_SONG_1);
      delay(1000);
    }
  }

  deltaTime.Run();
  delay(5);
}
