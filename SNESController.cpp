/*
 * 
 *  MIT License
 * 
 *  (C) Copyright 2024 Tim Böttiger
 * 
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to 
 *  deal in the Software without restriction, including without limitation the 
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in 
 *  all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.
 *  
 */

#include "SNESController.h"
#include <Arduino_DebugUtils.h>
#include <XInput.h>

char* regularButtonIdentifier[] = { "B", "Y", "Select", "Start", "Up", "Down", "Left", "Right", "A", "X", "L", "R", "Logo (emulated)" };
char* switchedButtonIdentifier[] = { "A", "Y", "Select", "Start", "Up", "Down", "Left", "Right", "B", "X", "L", "R", "Logo (emulated)" };

SNESController::SNESController(int switchedAB = 1)
{
  switchAB = switchedAB;
}

void SNESController::setup()
{
  #ifdef USB_XINPUT
    while (!XInput.connected()) {}  // wait for connection

    #ifdef LED_BUILTIN_RX
      digitalWrite(LED_BUILTIN_RX, LOW);
      delay(350);
      digitalWrite(LED_BUILTIN_RX, HIGH);
      delay(350);
      digitalWrite(LED_BUILTIN_RX, LOW);
      delay(350);
      digitalWrite(LED_BUILTIN_RX, HIGH);
      delay(350);
      digitalWrite(LED_BUILTIN_RX, LOW);
    #endif
  #endif
  
  XInput.setAutoSend(false);
  XInput.begin();

  // Setup NES / SNES latch and clock pins (2/3 or PD1/PD0)
  DDRD  |=  B00000011; // output
  PORTD &= ~B00000011; // low

  // Setup NES / SNES data pins (A0/A1 or PF6/PF7)
  DDRF  &= ~B11000000; // inputs
  PORTF |=  B11000000; // enable internal pull-ups
  
    /** Set DATA_CLOCK normally HIGH **/
  pinMode (DATA_CLOCK, OUTPUT);
  digitalWrite (DATA_CLOCK, LOW);
  
  /** Set DATA_LATCH normally LOW **/
  pinMode (DATA_LATCH, OUTPUT);
  digitalWrite (DATA_LATCH, LOW);

  /** Set DATA_SERIAL normally HIGH **/
  pinMode (DATA_SERIAL, INPUT_PULLUP);  
}

void SNESController::sendLatch()
{
  digitalWrite(DATA_LATCH, HIGH);
  __builtin_avr_delay_cycles(192);
  digitalWrite(DATA_LATCH, LOW);
  __builtin_avr_delay_cycles(72);
}

void SNESController::sendClock()
{
  digitalWrite(DATA_CLOCK, HIGH); 
  __builtin_avr_delay_cycles(96);
  digitalWrite(DATA_CLOCK, LOW);
  __builtin_avr_delay_cycles(72);
}

void SNESController::preFetch()
{
  #ifdef LED_BUILTIN_RX && LED_BUILTIN_TX
    digitalWrite(LED_BUILTIN_RX, !recorder.isRecording() ? HIGH : LOW);
    digitalWrite(LED_BUILTIN_TX, !deactivated ? HIGH : LOW);
  #endif
}

void SNESController::fetch()
{
  /** Latch for 12us **/
  sendLatch();

  /** Read data bit by bit from SR **/
  for (int id = 0; id < 16; id++) {
    if (id < SNES_BTN_NUM) {
      button[id].updateInput(!digitalRead(DATA_SERIAL));
      if (button[id].changed && !deactivated && recorder.isIdle())
      {
        char* buttonLabel = switchAB ? switchedButtonIdentifier[id] : regularButtonIdentifier[id];
        DEBUG_DEBUG("%s Pressed: %i", buttonLabel, button[id].pressed);
        DEBUG_DEBUG("%s Released: %i", buttonLabel, button[id].released);
        //DEBUG_DEBUG("%s Held: %i", buttonLabel, button[id].held);
        DEBUG_DEBUG("%s Duration: %i", buttonLabel, button[id].duration);
        DEBUG_DEBUG("%s Clicks: %i", buttonLabel, button[id].clicks);
      }
    }
    sendClock();
  }
}

void SNESController::postFetch()
{
  if (emulateLogoButton() || deactivated) return;

  if (!handleDeactivation())
    if (!handleAutoFire())
      if (!handleRecording())
        return;
}

bool SNESController::emulateLogoButton() {
  if (button[EMU_LOGO_BUTTON1].held && button[EMU_LOGO_BUTTON2].held)
  {
    button[SNES_EMU_LOGO].updateInput(true);
    return false;
  }
  button[SNES_EMU_LOGO].updateInput(false);
  return false;
}

bool SNESController::handleDeactivation() {
  int duration = DEACTIVATION_BUTTON_TIME * 1000;
  if (button[DEACTIVATION_BUTTON].duration > duration && button[DEACTIVATION_BUTTON].held)
  {
    int time_window_after_boot = DEACTIVATION_TIME_WINDOW * 1000;
    if (millis() < time_window_after_boot)
    {
      button[DEACTIVATION_BUTTON].reset();
      deactivated = true;

      DEBUG_WARNING("Modifications disabled until power reset");
      if (Debug.getDebugLevel() > -1) Debug.setDebugLevel(DBG_ERROR);
      return true;
    }
    else DEBUG_WARNING("Disabling of modifications only possible within the first %i seconds", time_window_after_boot);
  }
  return false;
}

bool SNESController::handleAutoFire() {
  int autoFireButtons[] = { SNES_A, SNES_B, SNES_X, SNES_Y, SNES_L, SNES_R };
  for (int id = 0; id < 6; ++id)
  {
    int autoFire = autoFireButtons[id];
    if (button[AUTOFIRE_BUTTON].held && button[autoFire].released)
    {
      button[autoFire].toggleMode();
      switch (button[autoFire].mode)
      {
      case MODE_NORMAL:
        DEBUG_INFO("%s Auto-Fire: Disabled", regularButtonIdentifier[autoFire]);
        break;
      case MODE_AUTOFIRE:
        DEBUG_INFO("%s Auto-Fire: Enabled", regularButtonIdentifier[autoFire]);
        break;
      }
      return true;
    }
  }
  return false;
}

bool SNESController::handleRecording() {
  int duration = CONTINUOUS_BUTTON_TIME * 1000;
  bool startRecording = button[PROGRAM_BUTTON].released && button[PROGRAM_BUTTON].clicks == PROGRAM_BUTTON_REC_CLICKS;
  if (recorder.isIdle())
  {
    if (startRecording)
    {
      recorder.startRecording();
      DEBUG_DEBUG("Recording: Started");
      return true;
    }
    else if (button[PROGRAM_BUTTON].released && button[PROGRAM_BUTTON].clicks == PROGRAM_BUTTON_PLAY_CLICKS && recorder.hasRecord() && button[PROGRAM_BUTTON].duration < MULTICLICK_TIMEOUT)
    {
      recorder.startPlayback();
      DEBUG_DEBUG("Playback: Started");
      return true;
    }
    else if (recorder.continuousPlayback && button[PROGRAM_BUTTON].held) 
    {
      recorder.startPlayback();
      DEBUG_DEBUG("Continuous Playback: Looped");
      return true;
    }
  }
  else if (recorder.isRecording())
  {
    if (button[PROGRAM_BUTTON].released && button[PROGRAM_BUTTON].clicks == PROGRAM_BUTTON_SAVE_CLICKS)
    {
      recorder.endRecording();
      recorder.continuousPlayback = false;

      int recordedButtons = recorder.countRecords();
      if (recordedButtons > 0) DEBUG_INFO("Recording: Finished (%i Buttonpresses saved)", recordedButtons);
      else DEBUG_DEBUG("Recording: Aborted");

      return true;
    }
    else if (button[PROGRAM_BUTTON].held && button[PROGRAM_BUTTON].duration > duration) 
    {
      recorder.endRecording();
      recorder.continuousPlayback = true;

      int recordedButtons = recorder.countRecords();
      if (recordedButtons > 0) DEBUG_INFO("Recording: Finished (%i Buttonpresses saved), Continuous Playback", recordedButtons);
      else DEBUG_DEBUG("Recording: Aborted");

      return true;
    }
  }
  else if (recorder.continuousPlayback && recorder.hasRecord() && startRecording)
  {
      recorder.startRecording();
      DEBUG_DEBUG("Recording: Started");
  }
  return false;
}

ControllerButton SNESController::get(int id)
{
  return button[id];
}

void SNESController::set(int id, ControllerButton buttonUpdate)
{
  button[id] = buttonUpdate;
}

void SNESController::preSubmit()
{
  int forcePlaybackButton = recorder.playback();
  for (int id = 0; id < SNES_BTN_NUM + 1; ++id)
  {
    bool emulatingLogoButton = button[SNES_EMU_LOGO].held && (id == EMU_LOGO_BUTTON1 || id == EMU_LOGO_BUTTON2);
    bool skipProgramButton = recorder.countRecords() > 0 && button[PROGRAM_BUTTON].held && id == PROGRAM_BUTTON;
    if (emulatingLogoButton || skipProgramButton)
    {
        button[id].ignore();
    }
    else 
    {
      button[id].process();
      if (recorder.isRecording())
      {
        if (button[id].pressed && id != PROGRAM_BUTTON) 
        {
          if (recorder.record(id)) DEBUG_DEBUG("%s Recording", regularButtonIdentifier[id]);
          else DEBUG_ERROR("%s Recording: FAILED", regularButtonIdentifier[id]);
        }
        button[id].ignore();
      }
      else if (forcePlaybackButton > -1)
      {
        if (id == forcePlaybackButton)
        {
          DEBUG_DEBUG("%s Playback", regularButtonIdentifier[id]);
          button[id].fire();
        }
        else button[id].ignore();
      }
    }
  }
}

void SNESController::submit()
{
  XInput.releaseAll();

  XInput.setButton(BUTTON_LOGO,      button[SNES_EMU_LOGO].output     );

  XInput.setButton(BUTTON_A,      button[switchAB ? SNES_A : SNES_B].output     );
  XInput.setButton(BUTTON_B,      button[switchAB ? SNES_B : SNES_A].output     );
  XInput.setButton(BUTTON_BACK,   button[SNES_SELECT].output);
  XInput.setButton(BUTTON_START,  button[SNES_START].output );

  XInput.setDpad(
    button[SNES_UP].output, 
    button[SNES_DOWN].output, 
    button[SNES_LEFT].output, 
    button[SNES_RIGHT].output,
    true
  );

  XInput.setButton(BUTTON_X,      button[SNES_X].output     );
  XInput.setButton(BUTTON_Y,      button[SNES_Y].output     );
  XInput.setButton(BUTTON_LB,     button[SNES_L].output     );
  XInput.setButton(BUTTON_RB,     button[SNES_R].output     );

  #ifdef USB_XINPUT
    XInput.send(); 
  #else 
    for (int id = 0; id < SNES_BTN_NUM + 1; ++id)
    {
      if (button[id].held && recorder.isIdle() && !recorder.isRecording())
      {
          DEBUG_INFO("%s (%s)", switchAB ? switchedButtonIdentifier[id] : regularButtonIdentifier[id], button[id].output ? "*" : " ");      
      }
    }
  #endif
}