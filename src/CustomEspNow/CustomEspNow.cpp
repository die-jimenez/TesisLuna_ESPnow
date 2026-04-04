//Example: https://docs.arduino.cc/tutorials/nano-esp32/esp-now/

#include "CustomEspNow.h"
#include <esp_now.h>
#include <WiFi.h>

// ====== DIRECCIÓN DE ENVÍO ======
uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };  //REPLACE WITH YOUR RECEIVER MAC Address

EspNowMessage myData;
EspNowMessage otherData;
EspNowMessage dataExample;
static esp_now_peer_info_t peerInfo;

// ====== CALLBACK ======
static OnReceiveCallback onReceiveCallback;
static OnSendCallback onSendCallback;
void EspNowRegisterOnReceive(OnReceiveCallback fn) {
  onReceiveCallback = fn;
}
void EspNowRegisterOnSend(OnSendCallback fn) {
  onSendCallback = fn;
}

void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.println("");
  Serial.println("--------------------------------------------");
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "MENSAJE ENVIADO" : "FALLO EL ENVIO DE MENSAJE");
}

void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  memcpy(&otherData, incomingData, sizeof(otherData));
  Serial.println("MENSAJE RECIBIDO");
  EspNowPrintReceiveData();

  if (otherData.publicPassword != PublicPassword) {
    Serial.println("Mensaje rechazado: contraseña incorrecta");
    return;
  }
  if (onReceiveCallback != nullptr) onReceiveCallback(otherData);
}
// ===============================================


void EspNowInit() {
  WiFi.mode(WIFI_STA);
  Serial.print("MAC Addres: ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //Register callbacks
  //esp_now_register_send_cb(OnDataSent);  //Send callack
  esp_now_register_recv_cb(OnDataRecv);  //Recive callback

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
  }
}

void EspNowSend(const EspNowMessage& data) {
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&data, sizeof(EspNowMessage));
  if (onSendCallback != nullptr) onSendCallback(data);

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
}

void EspNowSetMessage(int name, int stage, int statueEnabled, bool isReadyToHappyEnding, int destination) {
  Serial.println("");
  myData.name = name;
  myData.stage = stage;
  myData.statueEnabled = statueEnabled;
  myData.isReadyToHappyEnding = isReadyToHappyEnding;
  myData.publicPassword = PublicPassword;
  myData.destination = destination;
}

void EspNowSetAndSendMessage(int name, int stage, int statueEnabled, bool isReadyToHappyEnding, int destination) {
  EspNowSetMessage(name, stage, statueEnabled, isReadyToHappyEnding, destination);
  EspNowSend(myData);
}

void EspNowSendExample() {
  myData.name = 100;
  myData.stage = 1;
  myData.statueEnabled = 0;
  myData.isReadyToHappyEnding = false;
  myData.publicPassword = PublicPassword;
  myData.destination = 0;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&myData, sizeof(EspNowMessage));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
}

void EspNowPrintSendData() {
  Serial.println("MENSAJE ENVIADO");
  Serial.println("--------------------------------------------");

  Serial.print("Name: ");
  switch (myData.name) {
    case 0: Serial.println("SENSORS_HAPPY"); break;
    case 1: Serial.println("SENSORS_SAD"); break;
    case 2: Serial.println("AUDIO_SAD"); break;
    case 3: Serial.println("AUDIO_HAPPY"); break;
    default: Serial.println(myData.name); break;
  }
  Serial.print("Stage: ");
  switch (myData.stage) {
    case 0: Serial.println("STANDBY"); break;
    case 1: Serial.println("INTRO"); break;
    case 2: Serial.println("DESARROLLO"); break;
    case 3: Serial.println("FINAL"); break;
    default: Serial.println(myData.stage); break;
  }
  Serial.print("Current statue enabled: ");
  switch (myData.statueEnabled) {
    case 0: Serial.println("HAPPY_ENABLED"); break;
    case 1: Serial.println("SAD_ENABLED"); break;
    case 2: Serial.println("BOTH_ENABLED"); break;
    case 3: Serial.println("NONE (EMPTY)"); break;
    default: Serial.println(myData.statueEnabled); break;
  }
  Serial.print("Is ready to happy ending: ");
  Serial.println(myData.isReadyToHappyEnding ? "true" : "false");

  Serial.print("Destination: ");
  switch (myData.destination) {
    case 0: Serial.println("HAPPY"); break;
    case 1: Serial.println("SAD"); break;
    case 2: Serial.println("AUDIO_SAD"); break;
    case 3: Serial.println("AUDIO_HAPPY"); break;
    default: Serial.println(myData.destination); break;
  }
  Serial.println("--------------------------------------------");
  Serial.println();
}

void EspNowPrintReceiveData() {
  Serial.println("--------------------------------------------");

  Serial.print("Name: ");
  switch (otherData.name) {
    case 0: Serial.println("HAPPY"); break;
    case 1: Serial.println("SAD"); break;
    case 2: Serial.println("AUDIO_SAD"); break;
    case 3: Serial.println("AUDIO_HAPPY"); break;
    default: Serial.println(otherData.name); break;
  }
  Serial.print("Stage: ");
  switch (otherData.stage) {
    case 0: Serial.println("STANDBY"); break;
    case 1: Serial.println("INTRO"); break;
    case 2: Serial.println("DESARROLLO"); break;
    case 3: Serial.println("FINAL"); break;
    default: Serial.println(otherData.stage); break;
  }
  Serial.print("Current statue enabled: ");
  switch (otherData.statueEnabled) {
    case 0: Serial.println("HAPPY_ENABLED"); break;
    case 1: Serial.println("SAD_ENABLED"); break;
    case 2: Serial.println("BOTH_ENABLED"); break;
    case 3: Serial.println("NONE (EMPTY)"); break;
    default: Serial.println(otherData.statueEnabled); break;
  }
  Serial.print("Is ready to happy ending: ");
  Serial.println(otherData.isReadyToHappyEnding ? "true" : "false");

  Serial.print("Destination: ");
  switch (otherData.destination) {
    case 0: Serial.println("HAPPY"); break;
    case 1: Serial.println("SAD"); break;
    case 2: Serial.println("AUDIO_SAD"); break;
    case 3: Serial.println("AUDIO_HAPPY"); break;
    default: Serial.println(otherData.destination); break;
  }
  Serial.println("--------------------------------------------");
  Serial.println();
}