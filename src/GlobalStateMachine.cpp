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


  //No hace falta distinguir que espAudio es porque ya lo filtro desde el CustomEspNow
  bool isMyAudioMessage = (otherData.name == (int)StatueSetting::Name::AUDIO_HAPPY)
                          || (otherData.name == (int)StatueSetting::Name::AUDIO_SAD);

  if (isMyAudioMessage) {
    //NO ES CONFIABLE EL PIN BUSY DEL DF-PLAYER ASI QUE SE SIMULA EL FIN DEL AUDIO
    //Ahora el cmabio de estado se mantiene en el stateMachine
    // Serial.println("Termino el audio");
    // statueStateMachine->NotifyAudioFinished();
    return;
  }

  lastStage = stage;
  UpdateStage(otherData.stage);
  UpdateStatueEnabled(otherData.statueEnabled);

  if (stage == Stages::FINAL && lastStage == Stages::FINAL) {
    SyncFinalOnRecieve(otherData);
  }

  //Evita reinicio por inactividad
  resetTimer = 0;
}

void GlobalStateMachine::OnSendMessage(const EspNowMessage& myData) {
  if (myData.toAudio) return;
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
    statueStateMachine->ChangeState(StatueStateMachine::State::PETTING);
    //PlaySound(StatueSetting::AudiosTrack::TRACK_GOOD_ENDING);
  }
}
#pragma endregion


#pragma region->PettingStarted and AudioFinished(StatueStateMachine)
//=======================================================================================
void GlobalStateMachine::AudioFinishedSimulated() {
  switch (stage) {
    case (int)Stages::STANDBY:
      audioDuration = 5;
      break;
    case (int)Stages::INTRO:
      audioDuration = 10;
      break;
    case (int)Stages::DESARROLLO:
      audioDuration = 8;
      break;
    case (int)Stages::FINAL:
      audioDuration = 25;
      break;
  }

  timePlayingAudio += deltaTime->Get();
  if (timePlayingAudio >= audioDuration) {
    Serial.println("Termino el audio simulado");
    statueStateMachine->NotifyAudioFinished();
  }
}

void GlobalStateMachine::OnAudioFinished() {
  //Reinicio de la duracion del audio simulada
  timePlayingAudio = 0;

  if (stage != Stages::FINAL) {
    Stages nextStage = (Stages)((int)stage + 1);
    NextStageOrPassTurn(nextStage);
    EspNowPrintSendData();
    return;
  }

  bool goodEndingPlayed = happyOnGoodEnding && sadOnGoodEnding;
  if (goodEndingPlayed) {
    delay(2000);
    FullReset();
  } else {
    badEndingTimerActive = true;
    finalEndingTimer = 0;
  }
}

void GlobalStateMachine::OnPettingStarted() {
  switch (stage) {
    case (int)Stages::STANDBY:
    case (int)Stages::INTRO:
    case (int)Stages::DESARROLLO:
      MessageToAudio((int)stage, false);
      PrintEndingInfo();
      break;

    case (int)Stages::FINAL:
      // Marca que esta estatua fue activada
      if (statueSetting->name == StatueSetting::Name::SENSORS_HAPPY) happyOnGoodEnding = true;
      else sadOnGoodEnding = true;

      // Avisar al otro ESP de sensores que fue activado
      EspNowSetAndSendMessage(statueSetting->name, (int)stage, BOTH_ENABLED, false, false);

      // Mandar al propio audio
      bool goodEnding = happyOnGoodEnding && sadOnGoodEnding;
      EspNowSetAndSendMessage(statueSetting->name, (int)stage, NONE, goodEnding, true);

      EspNowPrintSendData();
      PrintEndingInfo();
      break;
  }
}

void GlobalStateMachine::MessageToAudio(int _stage, bool _canPlayGoodEnding) {
  EspNowSetAndSendMessage(statueSetting->name, _stage, NONE, _canPlayGoodEnding, statueSetting->ToAudio);
}
#pragma endregion


void GlobalStateMachine::NextStageOrPassTurn(GlobalStateMachine::Stages nextStage) {

  //Cambia de Stage si es "SAD",
  if (statueSetting->name == StatueSetting::Name::SENSORS_SAD) {
    switch (stage) {
      case (int)Stages::STANDBY:
      case (int)Stages::INTRO:
        stage = nextStage;
        EspNowSetAndSendMessage(statueSetting->name, (int)stage, HAPPY_ENABLED, false);
        break;
      case (int)Stages::DESARROLLO:
        stage = nextStage;
        EspNowSetAndSendMessage(statueSetting->name, (int)stage, BOTH_ENABLED, false);
        break;
    }
  }
  // o solo pasa de turno si es "HAPPY"
  else if (statueSetting->name == StatueSetting::Name::SENSORS_HAPPY) {
    switch (stage) {
      case (int)Stages::STANDBY:
      case (int)Stages::INTRO:
      case (int)Stages::DESARROLLO:
        EspNowSetAndSendMessage(statueSetting->name, (int)stage, SAD_ENABLED, false);
        break;
      case (int)Stages::FINAL:
        EspNowSetAndSendMessage(statueSetting->name, (int)stage, BOTH_ENABLED, false);
        break;
    }
  }
}

void GlobalStateMachine::UpdateTimerToPlayRandomSound(float interlapse) {
  if (stage == (Stages)Stages::STANDBY) {
    randomSoundTimer += deltaTime->Get();
    if (randomSoundTimer >= interlapse) {
      randomSoundTimer = 0;
      statueStateMachine->ChangeState(StatueStateMachine::State::PETTING);  //Actualizar luces
      //sensorsManager->DebugPetting();
      OnPettingStarted();  //Disparar audio y ciclo
    }
  } else {
    randomSoundTimer = 0;
  }
}


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
  EspNowSetAndSendMessage(statueSetting->name, Stages::STANDBY, BOTH_ENABLED, false);
}
#pragma endregion
