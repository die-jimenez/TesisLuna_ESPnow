#include "GlobalStateMachine.h"

// ==================== INIT
GlobalStateMachine::GlobalStateMachine() {
  deltaTime = nullptr;
}

void GlobalStateMachine::Init(int _statueName, StatueStateMachine* _statueStateMachine, DeltaTime* _deltaTime) {
  statueName = _statueName;
  deltaTime = _deltaTime;
  statueStateMachine = _statueStateMachine;
}


void GlobalStateMachine::OnReciveMessage(const EspNowMessage& otherData) {
  if (otherData.publicPassword != PublicPassword) {
    Serial.println("Llego un mensaje con la contraseña publica incorrecta");
    return;
  }
  //Cambia el Stage
  if (otherData.stage >= 0 && otherData.stage <= FINAL) {
    stage = (Stages)otherData.stage;
  }
  //Activa o desactiva las esculturas
  if (otherData.statueEnabled == BOTH_ENABLED) {
    statueStateMachine->SetCanInteract(true);
  } else {
    statueStateMachine->SetCanInteract(statueName == otherData.statueEnabled);
  }
  //Preprara el final feliz o no
  statueStateMachine->onHappyEnding = otherData.isReadyToHappyEnding;
}


void GlobalStateMachine::OnSendMessage(const EspNowMessage& myData) {
  bool shouldSenderKeepEnabled = myData.statueEnabled == BOTH_ENABLED;
  statueStateMachine->SetCanInteract(myData.statueEnabled == shouldSenderKeepEnabled);
}



// // ==================== PHASES
// void GlobalStateMachine::PrimerosMimitos() {
//   if (estaEscultura != syncData.esculturaInteractuable) return;

//   PlaySound(TRACK_SONG_1);

//   // La escultura TRISTE es quien avanza la fase
//   if (estaEscultura == TRISTE) {
//     syncData.faseActual = DESARROLLO;
//     syncData.esculturaInteractuable = FELIZ;
//   } else {
//     syncData.esculturaInteractuable = TRISTE;
//   }

//   SyncToOther();
// }

// void GlobalStateMachine::MimitosDesarrollo() {
//   if (estaEscultura != syncData.esculturaInteractuable) return;

//   PlaySound(TRACK_SONG_2);

//   if (estaEscultura == FELIZ) syncData.esculturaInteractuable = TRISTE;
//   else syncData.esculturaInteractuable = FELIZ;

//   // TODO: definir condicion para pasar a FINAL segun cantidad de canciones
//   // if (condicion) syncData.faseActual = FINAL;

//   SyncToOther();
// }

// void GlobalStateMachine::MimitosFinal() {
//   PlaySound(TRACK_BAD_ENDING);

//   if (estaEscultura == FELIZ) syncData.felizListoParaFinal = true;
//   else syncData.tristeListoParaFinal = true;

//   SyncToOther();

//   if (syncData.felizListoParaFinal && syncData.tristeListoParaFinal) {
//     EjecutarFinal();
//   }
// }

// void GlobalStateMachine::PrepararFinal() {
//   if (estaEscultura == FELIZ) syncData.felizListoParaFinal = true;
//   else syncData.tristeListoParaFinal = true;

//   if (syncData.felizListoParaFinal && syncData.tristeListoParaFinal) {
//     EspNowMessage msg;
//     msg.name = (estaEscultura == FELIZ) ? "feliz" : "triste";
//     msg.state = "EjecutarFinal";
//     msg.songIndex = 0;
//     msg.debug = "";
//     EspNowSend(msg);
//     EjecutarFinal();
//   }
// }

// void GlobalStateMachine::EjecutarFinal() {
//   PlaySound(TRACK_GOOD_ENDING);
//   // TODO: manejar luces de final con timerLuces y deltaTime
// }




// // ==================== SYNC
// void GlobalStateMachine::SyncToOther() {
//   EspNowMessage msg;
//   msg.name = (estaEscultura == FELIZ) ? "feliz" : "triste";
//   msg.state = "ActualizarEstados";
//   msg.songIndex = (int)syncData.faseActual;
//   msg.debug = (syncData.esculturaInteractuable == FELIZ) ? "feliz" : "triste";
//   EspNowSend(msg);
// }

// void GlobalStateMachine::OnReceiveData(const EspNowMessage& msg) {
//   if (msg.state == "ActualizarEstados") {
//     syncData.faseActual = (Fase)msg.songIndex;
//     syncData.esculturaInteractuable = (msg.debug == "feliz") ? FELIZ : TRISTE;
//   }
//   else if (msg.state == "PrepararFinal") {
//     PrepararFinal();
//   }
//   else if (msg.state == "EjecutarFinal") {
//     EjecutarFinal();
//   }
//}