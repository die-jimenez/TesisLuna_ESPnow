#include "CustomEspNow.h"
#include "AudioPlayer.h"

#include "src/DeltaTime/DeltaTime.h"
#include "src/TouchSensor/TouchSensor.h"


//Parametros Modificables
//===================================================
char* escultura = "TRISTE";                          //"FELIZ" || "TRISTE"
const float triggerToMimitos = 5.0;                  //Tiempo de interaccion para Mimito || INTERACION -> MIMITOS
const int minimo_Sensores_Activados_To_Mimitos = 2;  //Minimo de sensores activados para contar "Mimito" || INTERACION -> MIMITOS

//Pines usados
//===================================================
#define SENSOR_10_PIN 4  //Primer trio
#define SENSOR_11_PIN 16
#define SENSOR_12_PIN 17
#define SENSOR_20_PIN 36  //pin SVP (Solo input) //Segundo trio
#define SENSOR_21_PIN 39  //pin SVN (Solo input)
#define SENSOR_22_PIN 34
#define SENSOR_30_PIN 22  //Tercer trio
#define SENSOR_31_PIN 35
#define SENSOR_32_PIN 21
#define SENSOR_40_PIN 25  //Cuarto trio
#define SENSOR_41_PIN 33
#define SENSOR_42_PIN 32

// Configuración de pines para microSD
#define SD_MISO 19
#define SD_MOSI 23
#define SD_SCK 18
#define SD_SS 5  //PIN PROBLEMATICO

// Configuración de pines para I2S
#define I2S_LRC 14
#define I2S_BCLK 27
#define I2S_DOUT 26  //D-OUT = DIN

// Luces
#define LEDS_BOCA 13     //Se podrian unificar
#define LEDS_ENTORNO 12  //PIN PROBLEMATICO

//Pines que no puedo usar: 0, 2, 5, 12 https://www.luisllamas.es/en/esp32-hardware-details-pinout/

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
  SDcardInit(SD_SCK, SD_MISO, SD_MOSI, SD_SS);
  AudioInit(I2S_BCLK, I2S_LRC, I2S_DOUT);
}


void loop() {
  //EspNowSendExample();
  AudioUpdate();
  delay(5);
}
