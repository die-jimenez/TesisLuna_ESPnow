//Example: https://docs.arduino.cc/tutorials/nano-esp32/esp-now/

#include "CustomEspNow.h"
#include <esp_now.h>
#include <WiFi.h>

#include "../AudioPlayer/AudioPlayer.h"  //DEBUG ---> BORRAR LUEGO

// ====== DIRECCIÓN DE ENVÍO ======
uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };  //REPLACE WITH YOUR RECEIVER MAC Address

EspNowMessage otherData;
EspNowMessage dataExample;
static esp_now_peer_info_t peerInfo;

// ====== CALLBACK ======
static void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  memcpy(&otherData, incomingData, sizeof(otherData));
  EspNowPrintReceiveData();
  PlaySound("/1-L1.mp3");
}

void EspNowInit() {
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());


  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //Register callbacks
  esp_now_register_send_cb(OnDataSent);  //Send callack
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

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
}

void EspNowSendExample() {
  // strcpy(dataExample.a, "THIS IS A CHAR");
  // dataExample.b = random(1, 20);
  // dataExample.c = 1.2;
  // dataExample.d = false;

  dataExample.name = "cualquiera";
  dataExample.state = "cantando";
  dataExample.songIndex = 1;
  dataExample.debug = "Todo ok";

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&dataExample, sizeof(EspNowMessage));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
}

void EspNowPrintReceiveData() {
  Serial.print("Name: ");
  Serial.println(otherData.name);
  Serial.print("State: ");
  Serial.println(otherData.state);
  Serial.print("Song index: ");
  Serial.println(otherData.songIndex);
  Serial.print("Debug: ");
  Serial.println(otherData.debug);
  Serial.println();
}