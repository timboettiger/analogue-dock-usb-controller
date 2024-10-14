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

#include "ButtonPressRecorder.h"

ButtonPressRecorder::ButtonPressRecorder()
{
  recording = false;
  recordHeader = -1;
  playbackHeader = -1;
  continuousPlayback = false;
  memset(tape, 0, sizeof(tape));
}

bool ButtonPressRecorder::isIdle()
{
  return playbackHeader == -1 && !recording;
}

void ButtonPressRecorder::startRecording()
{
  recording = true;
  recordHeader = 0;
}

bool ButtonPressRecorder::record(int button)
{
  bool hasCapacity = (recordHeader > -1 && recordHeader < MAX_RECORDS);
  if (!hasCapacity || !recording) return false;

  tape[recordHeader] = button;
  recordHeader += 1;
  return true;
}

bool ButtonPressRecorder::isRecording()
{
  return recording;
}

void ButtonPressRecorder::endRecording()
{
  recording = false;
  if (recordHeader == 0) recordHeader = -1;
}

bool ButtonPressRecorder::hasRecord()
{
  return recordHeader > -1;
}

int ButtonPressRecorder::countRecords()
{
  if (recordHeader > -1) return recordHeader;
  else return 0;
}


void ButtonPressRecorder::startPlayback()
{
  if (isIdle() && hasRecord())
  {
    playbackHeader = 0;
  }
}

int ButtonPressRecorder::playback()
{
  if (recording) return -1;  
  if (playbackHeader > -1)
  {
    int playbackPosition = playbackHeader;
    if (playbackPosition < recordHeader)
    {
      playbackHeader += 1;
      return tape[playbackPosition];
    }
    else playbackHeader = -1;
  }
  return -1;
}

