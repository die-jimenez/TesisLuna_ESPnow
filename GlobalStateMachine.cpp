#include "GlobalStateMachine.h"

// ==================== INIT
GlobalStateMachine::GlobalStateMachine() {
  deltaTime = nullptr;
}

void GlobalStateMachine::Init(StatueSetting* _statueSetting, StatueStateMachine* _statueStateMachine, DeltaTime* _deltaTime) {
  statueSetting = _statueSetting;
  deltaTime = _deltaTime;
  statueStateMachine = _statueStateMachine;
  PrintInfo();
}

#pragma region OnRecieve&& OnSend Messages(Events on CustomEspNow)
//================================================================================================
void GlobalStateMachine::OnReciveMessage(const EspNowMessage& otherData) {
  if (!CheckPublicPassword(otherData.publicPassword)) return;
  UpdateStage(otherData.stage);
  UpdateStatueEnabled(otherData.statueEnabled);

  if (stage != Stages::FINAL) {
    SyncFinalOnRecieve(otherData);
  }

  //Evita reinicio por inactividad
  resetTimer = 0;
}

void GlobalStateMachine::OnSendMessage(const EspNowMessage& myData) {
  bool shouldSenderKeepEnabled = myData.statueEnabled == BOTH_ENABLED;
  statueStateMachine->SetCanInteract(shouldSenderKeepEnabled);
  //Evita reinicio por inactividad
  resetTimer = 0;
}
#pragma endregion


#pragma region Functions to OnRecieve&& OnSendMessages
//================================================================================================
bool GlobalStateMachine::CheckPublicPassword(int receivedPassword) {
  if (receivedPassword != PublicPassword) {
    Serial.println("Error: Contraseña pública incorrecta");
    return false;
  }
  return true;
}

void GlobalStateMachine::UpdateStage(int newStage) {
  if (newStage >= 0 && newStage <= (int)Stages::FINAL) {
    stage = (Stages)newStage;
  }
}

void GlobalStateMachine::UpdateStatueEnabled(int enabledMode) {
  statueEnabled = (StatuesEnabled)enabledMode;

  if (statueEnabled == BOTH_ENABLED) {
    statueStateMachine->SetCanInteract(true);
  } else {
    bool isMe = (statueSetting->name == (StatueSetting::Name)statueEnabled);
    statueStateMachine->SetCanInteract(isMe);
  }
}

void GlobalStateMachine::SyncFinalOnRecieve(const EspNowMessage& otherData) {

  if (otherData.name == (int)StatueSetting::Name::HAPPY) {
    happyOnGoodEnding = otherData.isReadyToHappyEnding;
  } else if (otherData.name == (int)StatueSetting::Name::SAD) {
    sadOnGoodEnding = otherData.isReadyToHappyEnding;
  }

  if (happyOnGoodEnding && sadOnGoodEnding) {
    Serial.println(">>> Sincronización completa: Iniciando FINAL FELIZ");
    PlaySound(StatueSetting::AudiosTrack::TRACK_GOOD_ENDING);

    // Aquí puedes añadir efectos visuales extra si quieres
    // statueStateMachine->TriggerHappyEndingLights();
  }
}
#pragma endregion






//Events on StatueStateMachine
//===================================================
void GlobalStateMachine::OnAudioFinished() {
  // El triste avanza el stage, el feliz solo pasa el turno
  switch (stage) {
    case (int)Stages::STANDBY:
      if (statueSetting->name == StatueSetting::Name::SAD) {
        stage = Stages::INTRO;
        EspNowSetAndSendMessage(statueSetting->name, (int)stage, HAPPY_ENABLED, false);
      } else EspNowSetAndSendMessage(statueSetting->name, (int)stage, SAD_ENABLED, false);
      break;
    case (int)Stages::INTRO:
      if (statueSetting->name == StatueSetting::Name::SAD) {
        stage = Stages::DESARROLLO;
        EspNowSetAndSendMessage(statueSetting->name, (int)stage, HAPPY_ENABLED, false);
      } else EspNowSetAndSendMessage(statueSetting->name, (int)stage, SAD_ENABLED, false);
      break;
    case (int)Stages::DESARROLLO:
      if (statueSetting->name == StatueSetting::Name::SAD) {
        stage = Stages::FINAL;
        EspNowSetAndSendMessage(statueSetting->name, (int)stage, BOTH_ENABLED, false);
      } else EspNowSetAndSendMessage(statueSetting->name, (int)stage, SAD_ENABLED, false);
      break;
    case (int)Stages::FINAL:
      break;
  }

  EspNowPrintSendData();
}

void GlobalStateMachine::OnPettingStarted() {
  switch (stage) {
    case (int)Stages::STANDBY:
      PlaySound(StatueSetting::AudiosTrack::TRACK_PURR_COMPLAIN);
      DelayForBusyUpdate();
      break;

    case (int)Stages::INTRO:
      PlaySound(StatueSetting::AudiosTrack::TRACK_SONG_1);
      DelayForBusyUpdate();
      break;

    case (int)Stages::DESARROLLO:
      PlaySound(StatueSetting::AudiosTrack::TRACK_SONG_2);
      DelayForBusyUpdate();
      break;

    case (int)Stages::FINAL:
      if (statueSetting->name == StatueSetting::Name::HAPPY) {
        happyOnGoodEnding = true;
      } else if (statueSetting->name == StatueSetting::Name::SAD) {
        sadOnGoodEnding = true;
      }
      // Sincronizamos con la otra para que también ejecute el final
      EspNowSetAndSendMessage(statueSetting->name, (int)stage, BOTH_ENABLED, true);
      if (happyOnGoodEnding && sadOnGoodEnding) PlayFinal(true);
      else PlayFinal(false);
      DelayForBusyUpdate();
      break;
  }
}

void GlobalStateMachine::PlayFinal(bool goodEnding) {

  if (goodEnding) {
    PlaySound(StatueSetting::AudiosTrack::TRACK_GOOD_ENDING);
    // Opcional: Podrías llamar a FullReset() después de un delay o cuando termine el audio en OnAudioFinished.
  } else {
    PlaySound(StatueSetting::AudiosTrack::TRACK_BAD_ENDING);
  }
}


void GlobalStateMachine::PrintInfo() {
  Serial.println();
  Serial.println("--------------------------------------------");

  Serial.print("Name: ");
  switch ((int)statueSetting->name) {
    case 0: Serial.println("HAPPY"); break;
    case 1: Serial.println("SAD"); break;
    default: Serial.println(myData.name); break;
  }
  Serial.print("Stage: ");
  switch ((int)stage) {
    case 0: Serial.println("STANDBY"); break;
    case 1: Serial.println("INTRO"); break;
    case 2: Serial.println("DESARROLLO"); break;
    case 3: Serial.println("FINAL"); break;
    default: Serial.println(myData.stage); break;
  }
  Serial.print("Current statue enabled: ");
  switch ((int)statueEnabled) {
    case 0: Serial.println("HAPPY_ENABLED"); break;
    case 1: Serial.println("SAD_ENABLED"); break;
    case 2: Serial.println("BOTH_ENABLED"); break;
    default: Serial.println(myData.statueEnabled); break;
  }
  Serial.print("Is HAPPY ready to good ending: ");
  Serial.println(happyOnGoodEnding ? "true" : "false");
  Serial.print("Is SAD ready to good ending: ");
  Serial.println(sadOnGoodEnding ? "true" : "false");
  Serial.println("--------------------------------------------");
  Serial.println();
}


//Reset whole experience
//==========================================================
void GlobalStateMachine::UpdateResetTimer(const float* _INACTIVITY_TIMEOUT) {
  if (stage != (Stages)Stages::STANDBY) {
    resetTimer += deltaTime->Get();
    if (resetTimer >= *_INACTIVITY_TIMEOUT) {
      FullReset();
    }
  }
}

void GlobalStateMachine::FullReset() {
  Serial.println("");
  Serial.println("SE REINCIIA TODO");

  stage = Stages::STANDBY;
  sadOnGoodEnding = false;
  happyOnGoodEnding = false;
  statueEnabled = StatuesEnabled::BOTH_ENABLED;
  statueStateMachine->ResetStatue();
  SendMessageToReset();
}

void GlobalStateMachine::SendMessageToReset() {
  EspNowSetAndSendMessage(statueSetting->name, Stages::STANDBY, BOTH_ENABLED, false);
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