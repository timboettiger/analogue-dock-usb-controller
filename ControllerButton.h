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

#ifndef CONTROLLERBUTTON_H
#define CONTROLLERBUTTON_H

#include "Arduino.h"
#include "Timer.h"

#define MULTICLICK_TIMEOUT    350  // milliseconds: time to register multiclick

#define MODE_NORMAL           0
#define MODE_AUTOFIRE         1

class ControllerButton {
public:
  int mode;
  bool input;
  bool changed;
  bool pressed;
  bool released;
  bool held;
  uint32_t duration;
  int clicks;
  bool output;

  ControllerButton();
  void updateInput(bool state);
  void setInput(bool state);
  void toggleMode();
  void reset();
  void process();
  void ignore();
  void fire();

private:
  int click_buffer;
  Timer clickStop;
  Timer durationStop;
};

#endif // CONTROLLERBUTTON_H