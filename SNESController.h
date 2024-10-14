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

#ifndef SNESCONTROLLER_H
#define SNESCONTROLLER_H

#include "GameConsoleController.h"
#include "ButtonPressRecorder.h"

/** BUTTONS **/
#define SNES_BTN_NUM  12
#define SNES_B        0
#define SNES_Y        1
#define SNES_SELECT   2
#define SNES_START    3
#define SNES_UP       4
#define SNES_DOWN     5
#define SNES_LEFT     6
#define SNES_RIGHT    7
#define SNES_A        8
#define SNES_X        9
#define SNES_L        10
#define SNES_R        11
#define SNES_EMU_LOGO SNES_BTN_NUM

#define EMU_LOGO_BUTTON1            SNES_SELECT   // id of auto fire mode button
#define EMU_LOGO_BUTTON2            SNES_START    // id of auto fire mode button

#define AUTOFIRE_BUTTON             SNES_SELECT   // id of auto fire mode button

#define PROGRAM_BUTTON              SNES_SELECT   // id of record program mode button
#define PROGRAM_BUTTON_REC_CLICKS   2             // how many clicks to start recording
#define PROGRAM_BUTTON_SAVE_CLICKS  2             // how many clicks to save program mode
#define PROGRAM_BUTTON_PLAY_CLICKS  1             // how many clicks to playback program
#define CONTINUOUS_BUTTON_TIME      3             // seconds: minimum time to 

/** DEACTIVATION **/
#define DEACTIVATION_BUTTON         SNES_SELECT   // id of deactivation button
#define DEACTIVATION_BUTTON_TIME    5             // seconds: minimum time to press 
#define DEACTIVATION_TIME_WINDOW    30            // seconds: time window after power on to deactivate mods

/** PINS **/
#define POW_5V        -1 // white
#define POW_GND       -1 // brown
#define DATA_CLOCK    19 // yellow
#define DATA_LATCH    20 // orange
#define DATA_SERIAL   21 // red

class SNESController : GameConsoleController {
public:
  SNESController(int switchedAB = 1);
  void setup();
  void preFetch();
  void fetch();
  void postFetch();
  ControllerButton get(int id);
  void set(int id, ControllerButton buttonUpdate);
  void preSubmit();
  void submit();

private:
  ControllerButton button[SNES_BTN_NUM + 1];
  ButtonPressRecorder recorder;
  bool deactivated;
  bool switchAB;

  bool emulateLogoButton();

  void sendLatch();
  void sendClock();

  bool handleDeactivation();
  bool handleAutoFire();
  bool handleRecording();
};

#endif // SNESCONTROLLER_H