#include <sys/_intsup.h>
#ifndef __CUSTOMESPNOW_H__
#define __CUSTOMESPNOW_H__
#include <Arduino.h>

const int PublicPassword = 1586;

// ====== STRUCT DEL MENSAJE ======
// Debe ser igual en emisor y receptor
struct EspNowMessage {
  int name;
  int stage;
  int statueEnabled;
  bool isReadyToHappyEnding;
  int publicPassword;
  int destination; 
};

extern EspNowMessage myData;
extern EspNowMessage otherData;
extern EspNowMessage dataExample; 

void EspNowInit();

void EspNowSend(const EspNowMessage& data);
void EspNowSetMessage(int name, int stage, int statueEnabled, bool isReadyToHappyEnding, int destination);
void EspNowSetAndSendMessage(int name, int stage, int statueEnabled, bool isReadyToHappyEnding, int destination);

void EspNowSendExample();
void EspNowPrintSendData();
void EspNowPrintReceiveData();

//Eventos
typedef void (*OnReceiveCallback)(const EspNowMessage& data);
typedef void (*OnSendCallback)(const EspNowMessage& data);
void EspNowRegisterOnReceive(OnReceiveCallback fn);
void EspNowRegisterOnSend(OnSendCallback fn);

#endif