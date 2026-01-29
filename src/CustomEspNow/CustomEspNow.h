#ifndef __CUSTOMESPNOW_H__
#define __CUSTOMESPNOW_H__
#include <Arduino.h>

// ====== STRUCT DEL MENSAJE ======
// Debe ser igual en emisor y receptor
struct EspNowMessage {
  String name; 
  String state;
  int songIndex;
  String debug;
};

extern EspNowMessage dataExample; 
extern EspNowMessage otherData;

void EspNowInit();
void EspNowSend(const EspNowMessage& data);
void EspNowSendExample();
void EspNowPrintReceiveData();

#endif