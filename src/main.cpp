#include <Wire.h>
#include <ArduinoJson.h>
#include "PresetDetector.h"
#include "PotentiometerController.h"
#include "ButtonController.h"
#include "ModeManager.h"
#include "RemController.h"


// --- Constants ---
#define BUTTON_PIN 9

static constexpr unsigned long BUTTON_PRESS_DURATION_MS = 500;
static constexpr unsigned long SETUP_DELAY_MS = 100;

static constexpr char MSG_START = '[';
static constexpr char MSG_END   = ']';

unsigned long pressStartTime = 0;
bool buttonActive = false;

PresetDetector presetDetector(A3);
int lastConfirmedPreset = 0;
ButtonController buttonController;
ModeManager modeManager;
RemController remController;


void sendResponse(const JsonDocument& doc) {
  Serial.print(MSG_START);
  serializeJson(doc, Serial);
  Serial.print(MSG_END);
  Serial.println();
}


void parseCommand(const String& jsonString) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    StaticJsonDocument<128> errorDoc;
    errorDoc["status"] = "error";
    errorDoc["message"] = "Invalid JSON";
    errorDoc["detail"] = error.c_str();
    sendResponse(errorDoc);
    return;
  }

  const char* command = doc["command"] | "";
  if (command[0] == '\0') {
    StaticJsonDocument<128> errorDoc;
    errorDoc["status"] = "error";
    errorDoc["message"] = "Missing or empty 'command' field";
    sendResponse(errorDoc);
    return;
  }

  if (strcmp(command, "set_volume") == 0) {
    int value = doc["value"] | -1;
    if (value >= 0 && value <= POT_MAX_VALUE) {
      bool success = setVolume(value);
      StaticJsonDocument<128> responseDoc;
      if (success) {
        responseDoc["status"] = "success";
        responseDoc["command"] = "set_volume";
        responseDoc["volume"] = value;
      } else {
        responseDoc["status"] = "error";
        responseDoc["message"] = "I2C communication failed (volume)";
      }
      sendResponse(responseDoc);
    } else {
      StaticJsonDocument<128> errorDoc;
      errorDoc["status"] = "error";
      errorDoc["message"] = "value must be 0-255";
      errorDoc["received"] = value;
      sendResponse(errorDoc);
    }
  }

  else if (strcmp(command, "set_volume_memo") == 0) {
    int value = doc["value"] | -1;
    if (value >= 0 && value <= POT_MAX_VALUE) {
      bool success = setVolumeMemory(value);
      StaticJsonDocument<128> responseDoc;
      if (success) {
        responseDoc["status"] = "success";
        responseDoc["command"] = "set_volume_memo";
        responseDoc["volume"] = value;
      } else {
        responseDoc["status"] = "error";
        responseDoc["message"] = "I2C communication failed (volume memo)";
      }
      sendResponse(responseDoc);
    } else {
      StaticJsonDocument<128> errorDoc;
      errorDoc["status"] = "error";
      errorDoc["message"] = "value must be 0-255";
      errorDoc["received"] = value;
      sendResponse(errorDoc);
    }
  }


  else if (strcmp(command, "set_mode") == 0) {
    const char* modeValue = doc["value"] | "standard";
    ModeManager::Mode newMode = ModeManager::modeFromName(modeValue);
    modeManager.setMode(newMode);

    StaticJsonDocument<128> responseDoc;
    responseDoc["status"] = "success";
    responseDoc["command"] = "set_mode";
    responseDoc["mode"] = modeManager.getModeName();
    sendResponse(responseDoc);
  }

  else if (strcmp(command, "set_bass_level") == 0) {
    int value = doc["value"] | -1;
    if (value >= 0 && value <= POT_MAX_VALUE) {
      bool success = setBassLevel(value);
      StaticJsonDocument<128> responseDoc;
      if (success) {
        responseDoc["status"] = "success";
        responseDoc["command"] = "set_bass_level";
        responseDoc["value"] = value;
      } else {
        responseDoc["status"] = "error";
        responseDoc["message"] = "I2C communication failed (value)";
      }
      sendResponse(responseDoc);
    } else {
      StaticJsonDocument<128> errorDoc;
      errorDoc["status"] = "error";
      errorDoc["message"] = "value must be 0-255";
      errorDoc["received"] = value;
      sendResponse(errorDoc);
    }
  }

  else if (strcmp(command, "change_preset") == 0) {
    digitalWrite(BUTTON_PIN, HIGH);
    pressStartTime = millis();
    buttonActive = true;
    StaticJsonDocument<128> responseDoc;
    responseDoc["status"] = "success";
    responseDoc["command"] = "change_preset";
    responseDoc["message"] = "Button press simulated (500ms)";
    sendResponse(responseDoc);
  }

  else if (strcmp(command, "set_is_enable_rem") == 0) {
    bool value = doc["value"].as<bool>();
    remController.enable(value);
    StaticJsonDocument<128> responseDoc;
    responseDoc["status"] = "success";
    responseDoc["command"] = "set_is_enable_rem";
    responseDoc["is_enable_rem"] = remController.isEnabled();
    sendResponse(responseDoc);
  }

  else if (strcmp(command, "get_preset") == 0) {
    StaticJsonDocument<128> eventDoc;
    eventDoc["command"] = "preset_changed";
    eventDoc["value"] = lastConfirmedPreset;
    sendResponse(eventDoc);
  }

  else if (strcmp(command, "get_mode") == 0) {
    StaticJsonDocument<128> responseDoc;
    responseDoc["status"] = "success";
    responseDoc["command"] = "get_mode";
    responseDoc["mode"] = modeManager.getModeName();
    sendResponse(responseDoc);
  }

  else if (strcmp(command, "button_down") == 0) {
    int value = doc["value"] | -1;
    if (value >= 1 && value <= 7) {
      buttonController.press(value);
      StaticJsonDocument<128> responseDoc;
      responseDoc["status"] = "success";
      responseDoc["command"] = "button_down";
      responseDoc["value"] = value;
      sendResponse(responseDoc);
    } else {
      StaticJsonDocument<128> errorDoc;
      errorDoc["status"] = "error";
      errorDoc["message"] = "button value must be 1-7";
      errorDoc["received"] = value;
      sendResponse(errorDoc);
    }
  }

  else if (strcmp(command, "button_up") == 0) {
    int value = doc["value"] | -1;
    if (value >= 1 && value <= 7) {
      buttonController.release(value);
      StaticJsonDocument<128> responseDoc;
      responseDoc["status"] = "success";
      responseDoc["command"] = "button_up";
      responseDoc["value"] = value;
      sendResponse(responseDoc);
    } else {
      StaticJsonDocument<128> errorDoc;
      errorDoc["status"] = "error";
      errorDoc["message"] = "button value must be 1-7";
      errorDoc["received"] = value;
      sendResponse(errorDoc);
    }
  }

  else if (strcmp(command, "ping") == 0) {
    StaticJsonDocument<128> responseDoc;
    responseDoc["status"] = "success";
    responseDoc["command"] = "pong";
    responseDoc["device"] = "Volume_Adapter";
    responseDoc["volume_range"] = "0-255";
    responseDoc["bass_range"] = "0-255";
    responseDoc["mode"] = modeManager.getModeName();
    sendResponse(responseDoc);
  } else {
    StaticJsonDocument<128> errorDoc;
    errorDoc["status"] = "error";
    errorDoc["message"] = "Unknown command";
    errorDoc["received_command"] = command;
    sendResponse(errorDoc);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(BUTTON_PIN, OUTPUT);
  digitalWrite(BUTTON_PIN, LOW);

  delay(SETUP_DELAY_MS);

  StaticJsonDocument<128> welcomeDoc;
  welcomeDoc["status"] = "ready";
  welcomeDoc["device"] = "Volume Adapter";
  welcomeDoc["protocol"] = "JUDI";
  welcomeDoc["mode"] = modeManager.getModeName();
  welcomeDoc["volume_range"] = "0-255";
  welcomeDoc["bass_range"] = "0-255";
  sendResponse(welcomeDoc);
  delay(SETUP_DELAY_MS);

  modeManager.begin();
  buttonController.begin();
  remController.begin();
  if (modeManager.getMode() == ModeManager::START_MIN_VALUE) {
    setVolume(1);
  }
}

void loop() {
  unsigned long now = millis();

  if (buttonActive && (now - pressStartTime >= BUTTON_PRESS_DURATION_MS)) {
    digitalWrite(BUTTON_PIN, LOW);
    buttonActive = false;
  }

  presetDetector.update();
  int currentPreset = presetDetector.getConfirmedPreset();
  if (currentPreset != lastConfirmedPreset) {
    lastConfirmedPreset = currentPreset;
  }

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() == 0) return;

    if (input.startsWith(String(MSG_START)) && input.endsWith(String(MSG_END))) {
      String content = input.substring(1, input.length() - 1);
      parseCommand(content);
    } else {
      StaticJsonDocument<128> errorDoc;
      errorDoc["status"] = "error";
      errorDoc["message"] = "Message must be enclosed in [ ]";
      sendResponse(errorDoc);
    }
  }
}
