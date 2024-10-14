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

#include "ControllerButton.h"

ControllerButton::ControllerButton()
{
  mode = MODE_NORMAL;
  input = false;
  changed = false;
  pressed = false;
  released = false;
  held = false;
  output = false;
  reset();
}

void ControllerButton::updateInput(bool state)
{
  changed = state != input;

  input = state;
  pressed = changed && input;
  released = changed && !input;
  held = !changed && input;

  if (pressed) durationStop.start();
  duration = durationStop.read();

  clickStop.pause();
  if (clickStop.read() > MULTICLICK_TIMEOUT)
  {
    clicks = 0;
  }

  if (released)
  {
    durationStop.stop();

    clicks += 1;
    clickStop.start();
  }
  else clickStop.resume();
}

void ControllerButton::setInput(bool state)
{
  mode = MODE_NORMAL;
  input = state;
  changed = false;
  pressed = state;
  released = false;
  held = state;
  output = state;
  reset();
}

void ControllerButton::toggleMode()
{
  switch(mode)
  {
    case MODE_NORMAL:
      mode = MODE_AUTOFIRE;
      break;
    case MODE_AUTOFIRE:
      mode = MODE_NORMAL;
      break;
  }
}

void ControllerButton::reset()
{
  clickStop.stop();
  clicks = 0;
  click_buffer = 0;
  duration = 0;
  clickStop.start();
}

void ControllerButton::process()
{
  switch(mode)
  {
    case MODE_NORMAL:
      output = input;
      break;
    case MODE_AUTOFIRE:
      output = input ? !output : false;
      break;
  }
}

void ControllerButton::ignore()
{
  output = false;
}

void ControllerButton::fire()
{
  output = true;
}