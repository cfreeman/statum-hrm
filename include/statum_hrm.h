/*
 * Copyright (c) Clinton Freeman 2019
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _STATUM_HRM_ACH_
#define _STATUM_HRM_ACH_

#include <Arduino.h>

#include "SmoothedStruct.h"

typedef struct State_struct (*StateFn)(const struct State_struct *currentState,
                                       unsigned long currentTime,
                                       long irValue);

typedef struct State_struct {
  String url;              // The URL to use when informing the server of the current heart rate.
  unsigned long lastPulse; // The last time in milliseconds that a pulse was detected.
  SmoothedValues *pulses;  // A pointer to a collection of the most recent pulse durations.

  StateFn update;          // The function to use when updating the state of the statum hrm.
} State;


// SampleMode updates the state of the Statum heart rate monitor with the latest data
// from the MAX30102 heart rate sensor.
State SampleMode(const State_struct *currentState,
                 unsigned long currentTime,
                 long irValue);

// BroadcastMode informs the server about the current state of the Statum heart rate
// monitor.
State BroadcastMode(const State_struct *currentState,
                    unsigned long currentTime,
                    long irValue);


#endif