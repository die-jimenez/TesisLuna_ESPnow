//Library versions are detailed in the README
#include "Arduino.h"
#include "src/CustomEspNow/CustomEspNow.h"
#include "src/AudioPlayer/AudioPlayer.h"
#include "src/StatueSetting/StatueSetting.h"
#include "src/Lights/Lights.h"

#include "StatueStateMachine.h"
#include "GlobalStateMachine.h"

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
#define BUSY 4

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


//Others
//===================================================
Lights lights(LEDS_BOCA, LEDS_ENTORNO);
DeltaTime deltaTime;

//States machines
//===================================================
StatueStateMachine statueStateMachine;
GlobalStateMachine globalStateMachine;

//Debug
//===================================================
int contadorInteraccion;
int contadorMimitos;


//Parametros Modificables
//===================================================
StatueSetting statueSetting(StatueSetting::Name::HAPPY);
const int MIN_SENSORS_ACTIVE_TO_PET = 1;  //Minimo de sensores activados para contar "Mimito" || INTERACION -> MIMITOS
const float pettingTriggerTime = 5.0;     //Tiempo de interaccion para Mimito || INTERACION -> MIMITOS
const uint8_t SENSORS_COUNT = 1;          //Sensores activos. Evita pinouts de más
const uint8_t resetTriggerTime = 6000 ;

SensorsManager sensorsManager(sensors, SENSORS_COUNT);




void setup() {
  Serial.begin(115200);

  //Global state machine
  globalStateMachine.Init(&statueSetting, &statueStateMachine, &deltaTime);

  //State Machine
  statueStateMachine.Init(&statueSetting, &sensorsManager, &lights, &deltaTime);
  statueStateMachine.RegisterOnPettingStarted(OnPettingStarted);
  statueStateMachine.RegisterOnAudioFinished(OnAudioFinished);
  statueStateMachine.SetCanInteract(true);

  //Esp now
  EspNowInit();
  EspNowRegisterOnSend(OnSendData);
  EspNowRegisterOnReceive(OnReceiveData);

  //Audio module
  Mp3ModuleInit(RXD2, TXD2, BUSY);
  delay(500);
  PlaySound(1);

  //Sensors init
  sensorsManager.SetShowDebug(false);
  sensorsManager.InitSensors();

  //Leds
  lights.Init();
  lights.TurnOn(true);

  delay(1000);
  Serial.println("setup is complete");
  Serial.println("--------------------------------------------");
}


void loop() {
  //Sensors update
  sensorsManager.UpdateSensors(deltaTime.Get());

  //Maquina de estados de la estatua
  //===================================================
  if (statueStateMachine.GetCanInteract()) {
    if (statueStateMachine.state == StatueStateMachine::IDLE) {
      statueStateMachine.UpdateIdle();
    }  //
    else if (statueStateMachine.state == StatueStateMachine::INTERACTING) {
      statueStateMachine.UpdateInteraction(pettingTriggerTime, MIN_SENSORS_ACTIVE_TO_PET);
    }  //
    else if (statueStateMachine.state == StatueStateMachine::PETTING) {
      statueStateMachine.UpdatePetting();
    }
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


//Estos son los metodos custom para utilizar CustomEspNow
void OnReceiveData(const EspNowMessage& data) {
  globalStateMachine.OnReciveMessage(data);
}

void OnSendData(const EspNowMessage& data) {
  globalStateMachine.OnSendMessage(data);
}

void OnAudioFinished() {
  globalStateMachine.OnAudioFinished();
}
void OnPettingStarted() {
  globalStateMachine.OnPettingStarted();
}

void ResetExperience() {
  statueStateMachine.ResetAll();
  globalStateMachine.Reset();
  globalStateMachine.SendMessageToReset();
}
