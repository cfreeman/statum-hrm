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
#include <Arduino.h>
#include <Wire.h>
#include <MAX30105.h>
#include <heartRate.h>
#include <ESP8266WiFi.h>

MAX30105 particleSensor;

#include "configuration.h"
#include "statum_hrm.h"

SmoothedValues* pulses; // The most recently calculated pulse widths.
State state;            // The current state of the statum heart rate monitor.

void setup() {
	Serial.begin(9600);

  Serial.print("Initialising Particle Sensor...");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30101 was not found. Please check wiring/power. ");
    while (1);
  }
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  Serial.println(" Done!");

  // Connect to WiFi.
  Serial.print("Connecting to Wifi");
  WiFi.disconnect();          // Prevent need for powercyle after upload.
  WiFi.mode(WIFI_STA);        // Use DHCP to connect and obtain IP Address.
  WiFi.begin(SSID, PASSWORD);

  // Wait until we have connected to the WiFi AP.
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  Serial.println(" Done!");

  pulses = new_smoothed(7); // Singleton that exists for the entire running time.

  // Initalise the state of the Statum HRM.
  state = State{"", millis(), pulses, &SampleMode};
}

void loop() {
  state = state.update(&state, millis(), particleSensor.getIR());
}