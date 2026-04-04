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

#pragma region->Send or recieve messages(CustomEspNow)
//================================================================================================
void GlobalStateMachine::OnReciveMessage(const EspNowMessage& otherData) {
  if (!CheckPublicPassword(otherData.publicPassword)) return;

  bool isFromMyAudio = (otherData.name == (int)StatueSetting::Name::AUDIO_HAPPY)
                       || (otherData.name == (int)StatueSetting::Name::AUDIO_SAD);

  if (isFromMyAudio) {
    Serial.println("El audio termino");
    OnAudioFinished();
    return;
  }

  //PARCHE: Ahora, la preparacion final se da por el stage, y no por el isReadyToHappyEnding.
  //esto provoca que el cambio de fase active el final en SAD. Esto busca evitar que se dispare de esa
  bool wasOnDesarrollo = stage == Stages::DESARROLLO;

  UpdateStage(otherData.stage);
  UpdateStatueEnabled(otherData.statueEnabled);

  if (stage == Stages::FINAL && !wasOnDesarrollo)
    SyncFinalOnRecieve(otherData);

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


#pragma region->Functions to send or recieve messages
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
  if (otherData.stage != (int)Stages::FINAL) return;

  if (otherData.name == (int)StatueSetting::Name::SENSORS_HAPPY) {
    happyOnGoodEnding = true;
  } else if (otherData.name == (int)StatueSetting::Name::SENSORS_SAD) {
    sadOnGoodEnding = true;
  }
  PrintEndingInfo();

  if (happyOnGoodEnding && sadOnGoodEnding) {
    Serial.println(">>> Sincronización completa: Iniciando FINAL FELIZ");
    MessageToAudio((int)stage, true);
    //PlaySound(StatueSetting::AudiosTrack::TRACK_GOOD_ENDING);
  }
}
#pragma endregion


#pragma region->PettingStarted and AudioFinished(StatueStateMachine)
//=======================================================================================
void GlobalStateMachine::OnAudioFinished() {
  // Serial.println("SE REINICIO A IDLE AAAAAAAAAAAAAAAAAAAAAA");
  // statueStateMachine->ResetInteractionState();
  // Serial.println(statueStateMachine->state);

  if (stage == Stages::FINAL) {
    bool goodEndingPlayed = happyOnGoodEnding && sadOnGoodEnding;
    if (goodEndingPlayed) {
      delay(2000);
      FullReset();
    } else {
      statueStateMachine->ResetInteractionState();
      badEndingTimerActive = true;
      finalEndingTimer = 0;
    }
    return;
  }

  statueStateMachine->ResetInteractionState();
  Stages nextStage = (Stages)((int)stage + 1);
  NextStageOrPassTurn(nextStage);
  EspNowPrintSendData();
}

void GlobalStateMachine::OnPettingStarted() {
  if (stage != (int)Stages::FINAL) {
    MessageToAudio((int)stage, false);
    return;  //Espera notificacion del audio
  }
  //Final...
  if (statueSetting->name == StatueSetting::Name::SENSORS_HAPPY)
    happyOnGoodEnding = true;
  else sadOnGoodEnding = true;

  // Avisar al otro ESP de sensores que fue activado
  MessageToOtherSensor((int)Stages::FINAL, BOTH_ENABLED, false);

  // Mandar al propio audio
  bool canPlayGoodEnding = happyOnGoodEnding && sadOnGoodEnding;
  MessageToAudio((int)Stages::FINAL, canPlayGoodEnding);

  EspNowPrintSendData();
  PrintEndingInfo();
}

#pragma endregion


void GlobalStateMachine::NextStageOrPassTurn(GlobalStateMachine::Stages nextStage) {

  //Cambia de Stage si es "SAD",
  if (statueSetting->name == StatueSetting::Name::SENSORS_SAD) {
    switch (stage) {
      case (int)Stages::STANDBY:
      case (int)Stages::INTRO:
        stage = nextStage;
        MessageToOtherSensor((int)stage, HAPPY_ENABLED, false);
        break;
      case (int)Stages::DESARROLLO:
        stage = nextStage;
        MessageToOtherSensor((int)Stages::FINAL, BOTH_ENABLED, false);
        break;
    }
  }
  // o solo pasa de turno si es "HAPPY"
  else if (statueSetting->name == StatueSetting::Name::SENSORS_HAPPY) {
    switch (stage) {
      case (int)Stages::STANDBY:
      case (int)Stages::INTRO:
      case (int)Stages::DESARROLLO:
        MessageToOtherSensor((int)Stages::FINAL, SAD_ENABLED, false);
        break;
      case (int)Stages::FINAL:
        MessageToOtherSensor((int)Stages::FINAL, BOTH_ENABLED, false);
        break;
    }
  }
}



void GlobalStateMachine::MessageToAudio(int _stage, bool _canPlayGoodEnding) {
  int myAudio;
  if (statueSetting->name == (int)StatueSetting::Name::SENSORS_HAPPY)
    myAudio = StatueSetting::Name::AUDIO_HAPPY;
  else if (statueSetting->name == (int)StatueSetting::Name::SENSORS_SAD)
    myAudio = StatueSetting::Name::AUDIO_SAD;
  else Serial.println("EL NAME DE ESTE ESP NO CORRESPONDE A LOS DISPONIBLES");

  EspNowSetAndSendMessage(statueSetting->name, _stage, NONE, _canPlayGoodEnding, myAudio);
}

void GlobalStateMachine::MessageToOtherSensor(int _stage, int _statuesEnabled, bool _canPlayGoodEnding) {
  int otherSensorController;
  if (statueSetting->name == (int)StatueSetting::Name::SENSORS_HAPPY)
    otherSensorController = StatueSetting::Name::SENSORS_SAD;
  else if (statueSetting->name == (int)StatueSetting::Name::SENSORS_SAD)
    otherSensorController = StatueSetting::Name::SENSORS_HAPPY;
  else Serial.println("EL NAME DE ESTE ESP NO CORRESPONDE A LOS DISPONIBLES");

  EspNowSetAndSendMessage(statueSetting->name, _stage, _statuesEnabled, _canPlayGoodEnding, otherSensorController);
}



// void GlobalStateMachine::PlayFinal(bool goodEnding) {

//   if (goodEnding) {
//     //PlaySound(StatueSetting::AudiosTrack::TRACK_GOOD_ENDING);
//     EspNowSetAndSendMessage(statueSetting->name, (int)stage, NONE, true, statueSetting->ToAudio);
//     // Opcional: Podrías llamar a FullReset() después de un delay o cuando termine el audio en OnAudioFinished.
//   } else {
//     EspNowSetAndSendMessage(statueSetting->name, (int)stage, NONE, true, statueSetting->ToAudio);
//   }
















void GlobalStateMachine::PrintInfo() {
  Serial.println();
  Serial.println("--------------------------------------------");

  Serial.print("Name: ");
  switch ((int)statueSetting->name) {
    case 0: Serial.println("SENSORS_HAPPY"); break;
    case 1: Serial.println("SENSORS_SAD"); break;
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

void GlobalStateMachine::PrintEndingInfo() {
  Serial.print("Is HAPPY ready to good ending: ");
  Serial.println(happyOnGoodEnding ? "true" : "false");
  Serial.print("Is SAD ready to good ending: ");
  Serial.println(sadOnGoodEnding ? "true" : "false");
}


#pragma region->Reset whole experience
//==========================================================
void GlobalStateMachine::UpdateResetTimer(const float* _INACTIVITY_TIMEOUT, const float* _BAD_ENDING_RESET_TIMEOUT) {
  if (stage != (Stages)Stages::STANDBY) {
    resetTimer += deltaTime->Get();
    if (resetTimer >= *_INACTIVITY_TIMEOUT) {
      FullReset();
    }
  }

  if (badEndingTimerActive) {
    finalEndingTimer += deltaTime->Get();
    if (finalEndingTimer >= *_BAD_ENDING_RESET_TIMEOUT) {
      badEndingTimerActive = false;
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
  badEndingTimerActive = false;
  finalEndingTimer = 0;

  statueEnabled = StatuesEnabled::BOTH_ENABLED;
  statueStateMachine->ResetStatue();
  SendMessageToReset();
}

void GlobalStateMachine::SendMessageToReset() {
  MessageToOtherSensor((int)Stages::STANDBY, BOTH_ENABLED, false);
}
#pragma endregion
