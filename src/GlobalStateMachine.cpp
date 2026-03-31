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

  bool isAudioMessage =
    otherData.name == (int)StatueSetting::Name::AUDIO_HAPPY || otherData.name == (int)StatueSetting::Name::AUDIO_SAD;

  if (isAudioMessage) {
    //HandleAudioMessage(otherData);
    return;
  }

  UpdateStage(otherData.stage);
  UpdateStatueEnabled(otherData.statueEnabled);

  if (stage == Stages::FINAL) {
    SyncFinalOnRecieve(otherData);
    PrintEndingInfo();
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

  if (otherData.name == (int)StatueSetting::Name::SENSORS_HAPPY) {
    happyOnGoodEnding = otherData.isReadyToHappyEnding;
  } else if (otherData.name == (int)StatueSetting::Name::SENSORS_SAD) {
    sadOnGoodEnding = otherData.isReadyToHappyEnding;
  }

  if (happyOnGoodEnding && sadOnGoodEnding) {
    Serial.println(">>> Sincronización completa: Iniciando FINAL FELIZ");
    EspNowSetAndSendMessage(statueSetting->name, (int)stage, NONE, true, statueSetting->ToAudio);
    //PlaySound(StatueSetting::AudiosTrack::TRACK_GOOD_ENDING);

    // Aquí puedes añadir efectos visuales extra si quieres
    // statueStateMachine->TriggerHappyEndingLights();
  }
}
#pragma endregion


#pragma region->PettingStarted and AudioFinished(StatueStateMachine)
//=======================================================================================
void GlobalStateMachine::OnAudioFinished() {
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
      EspNowSetAndSendMessage(statueSetting->name, (int)stage, NONE, false, statueSetting->ToAudio);
      //PlaySound(StatueSetting::AudiosTrack::TRACK_PURR_COMPLAIN);
      DelayForBusyUpdate();
      break;

    case (int)Stages::INTRO:
      EspNowSetAndSendMessage(statueSetting->name, (int)stage, NONE, false, statueSetting->ToAudio);
      //PlaySound(StatueSetting::AudiosTrack::TRACK_SONG_1);
      DelayForBusyUpdate();
      break;

    case (int)Stages::DESARROLLO:
      EspNowSetAndSendMessage(statueSetting->name, (int)stage, NONE, false, statueSetting->ToAudio);
      //PlaySound(StatueSetting::AudiosTrack::TRACK_SONG_2);
      DelayForBusyUpdate();
      break;

    case (int)Stages::FINAL:
      if (statueSetting->name == StatueSetting::Name::SENSORS_HAPPY) {
        happyOnGoodEnding = true;
      } else if (statueSetting->name == StatueSetting::Name::SENSORS_SAD) {
        sadOnGoodEnding = true;
      }
      // Sincronizamos con la otra para que también ejecute el final
      EspNowSetAndSendMessage(statueSetting->name, (int)stage, BOTH_ENABLED, true);
      EspNowPrintSendData();
      PrintEndingInfo();


      if (happyOnGoodEnding && sadOnGoodEnding) PlayFinal(true);
      else PlayFinal(false);
      DelayForBusyUpdate();
      break;
  }
}

#pragma endregion


void GlobalStateMachine::PlayFinal(bool goodEnding) {

  if (goodEnding) {
    //PlaySound(StatueSetting::AudiosTrack::TRACK_GOOD_ENDING);
    EspNowSetAndSendMessage(statueSetting->name, (int)stage, NONE, true, statueSetting->ToAudio);
    // Opcional: Podrías llamar a FullReset() después de un delay o cuando termine el audio en OnAudioFinished.
  } else {
    EspNowSetAndSendMessage(statueSetting->name, (int)stage, NONE, true, statueSetting->ToAudio);
  }
}

void GlobalStateMachine::NextStageOrPassTurn(GlobalStateMachine::Stages nextStage) {
  //Cambia de Stage si es "SAD", o solo pasa de turno si es "HAPPY"
  if (statueSetting->name == StatueSetting::Name::SENSORS_SAD) {
    stage = nextStage;
    EspNowSetAndSendMessage(statueSetting->name, (int)stage, HAPPY_ENABLED, false);
  } else EspNowSetAndSendMessage(statueSetting->name, (int)stage, SAD_ENABLED, false);
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
