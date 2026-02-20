#ifndef __CUSTOMESPNOW_H__
#define __CUSTOMESPNOW_H__
#include <Arduino.h>

// ====== STRUCT DEL MENSAJE ======
// Debe ser igual en emisor y receptor
struct EspNowMessage {
  int name;
  int stage;
  int statueEnabled;
  bool isReadyToHappyEnding;
};

extern EspNowMessage myData;
extern EspNowMessage otherData;
extern EspNowMessage dataExample; 

void EspNowInit();
void EspNowSend(const EspNowMessage& data);
void SetMessage(int name, int stage, int statueEnabled, bool isReadyToHappyEnding);
void EspNowSendExample();
void EspNowPrintReceiveData();

#endif