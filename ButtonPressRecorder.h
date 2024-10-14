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

#ifndef BUTTONPRESSRECORDER_H
#define BUTTONPRESSRECORDER_H

#include "Arduino.h"

#define MAX_RECORDS   255

class ButtonPressRecorder {
public:
  ButtonPressRecorder();

  bool isIdle();

  bool canRecord();
  void startRecording();
  bool record(int button);
  bool isRecording();
  void endRecording();
  bool hasRecord();
  int countRecords();

  void startPlayback();
  int playback();

  bool continuousPlayback;

private:
  bool recording;
  int recordHeader;
  int playbackHeader;
  int tape[MAX_RECORDS];
};
#endif // BUTTONPRESSRECORDER_H