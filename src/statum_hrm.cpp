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
#include <ESP8266HTTPClient.h>
#include <heartRate.h>

#include "configuration.h"
#include "statum_hrm.h"

State SampleMode(const State_struct *currentState, unsigned long currentTime, long irValue){
  if (irValue > 5000 && checkForBeat(irValue) == true) {
    float deltaP = ((currentTime - currentState->lastPulse) / 1000.0f);

    // Min heart rate is set at 50 bpm (sometimes the sensor misses a beat or two.)
    if (deltaP < 1.2) {
      add_value(currentState->pulses, deltaP);
    }

    // Use the average of the most recently calculated pulse durations to determine
    // the current heart rate in beats per minute (to smooth out any sampling noise
    // from the sensor).
    int bpm = (int)(60.0 / currentState->pulses->smoothed_value);
    String url = String(SERVER_ADDR) + "pulse?id="+SENSOR_ID+
                                       "&bpm=" + bpm;

    return State{url,
                 currentTime,
                 currentState->pulses,
                 &BroadcastMode};
  }

  return *currentState;
}

State BroadcastMode(const State_struct *currentState, unsigned long currentTime, long irValue) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, currentState->url);
  http.GET();
  http.end();

  return State{"",
               currentState->lastPulse,
               currentState->pulses,
               &SampleMode};
}
