#ifndef __CUSTOMESPNOW_H__
#define __CUSTOMESPNOW_H__

#include <Arduino.h>

// ====== STRUCT DEL MENSAJE ======
// Debe ser igual en emisor y receptor
struct EspNowMessage {
  char a[32];
  int b;
  float c;
  bool d;
};

extern EspNowMessage dataExample; 
extern EspNowMessage otherData;

void EspNowInit();
void EspNowSend(const EspNowMessage& data);
void EspNowSendExample();

#endif