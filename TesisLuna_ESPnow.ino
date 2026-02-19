//Library versions are detailed in the README
#include "Arduino.h"
#include "src/CustomEspNow/CustomEspNow.h"
#include "src/AudioPlayer/AudioPlayer.h"
#include "src/Statue/Statue.h"
#include "StatueStateMachine.h"

#include "src/DeltaTime/DeltaTime.h"
#include "src/TouchSensor/TouchSensor.h"
#include "src/SensorsManager/SensorsManager.h"


//Pines usados
//===================================================
#define SENSOR_10_PIN 27  //Verificar estos pines <----------------
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
#define RXD2 16  // esp receiver (rx) --> module transciever (tx)
#define TXD2 17  // D2 = segundo puerto serial, pero es solo nombre
#define BUSY 5


//Maquina de estados
//===================================================
// #define ESPERANDO 0
// #define HAY_INTERACCION 1
// #define HAY_MIMITOS 2
// int state = 0;
// float cronometroToMimitos;


//Sensors
//===================================================
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
const uint8_t SENSORS_COUNT = 1;  //Sensores activos. Evita pinouts de más
SensorsManager sensorsManager(sensors, SENSORS_COUNT);

//Others
//===================================================
DeltaTime deltaTime;
StatueStateMachine statueStateMachine;

//Debug
//===================================================
int contadorInteraccion;
int contadorMimitos;


//Parametros Modificables
//===================================================
Statue statue(Statue::StatueName::happy, LEDS_BOCA);
const int MIN_SENSORS_ACTIVE_TO_PET = 1;  //Minimo de sensores activados para contar "Mimito" || INTERACION -> MIMITOS
const float pettingTriggerTime = 5.0;       //Tiempo de interaccion para Mimito || INTERACION -> MIMITOS




void setup() {
  Serial.begin(115200);
  //Mp3ModuleInit(RXD2, TXD2);
  PlaySound(statue.TRACK_SONG_1);

  EspNowInit();

  //Sensors init
  sensorsManager.SetShowDebug(false);
  sensorsManager.InitSensors();

  //Leds
  statue.InitLeds();
  statue.TurnOnLEDs(true);

  //State Machine
  statueStateMachine.Init(&statue, &sensorsManager, &deltaTime);
  statueStateMachine.GetAudioBusyPin(BUSY);

  delay(1000);
  Serial.println("setup is complete");
  Serial.println("--------------------------------------------");
}


void loop() {
  //EspNowSendExample();

  //Sensors update
  sensorsManager.UpdateSensors(deltaTime.Get());

  //Maquina de estados de la estatua
  //===================================================
  if (statueStateMachine.state == StatueStateMachine::IDLE) {
    statueStateMachine.UpdateIdle();
  }  //
  else if (statueStateMachine.state == StatueStateMachine::INTERACTING) {
    statueStateMachine.UpdateInteraction(pettingTriggerTime, MIN_SENSORS_ACTIVE_TO_PET);
    //PlaySound(statue.TRACK_SONG_1);
    delay(5000);//Debug
  }  //
  else if (statueStateMachine.state == StatueStateMachine::PETTING) {
    statueStateMachine.UpdatePetting();
    //PlaySound(statue.TRACK_SONG_2);
    delay(5000);//Debug
  }

  //Debug
  //===================================================
  // for (int i = 0; i < SENSORS_COUNT; i++) {
  //   if (sensors[i].isTouchStable()) {
  //     EspNowSendExample();
  //     PlaySound(statue.TRACK_SONG_1);
  //     delay(1000);
  //   }
  // }

  deltaTime.Run();
  delay(5);
}

