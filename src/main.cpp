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

MAX30105 particleSensor;

#include <ESP8266WiFi.h>

#include "configuration.h"

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

unsigned long lastPrint = 0;

void setup() {
	Serial.begin(9600);

  Serial.print("Initialising Particle Sensor...");
  // Wire.begin();
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
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

  // Initalise the state of the Statum HRM.
  //unsigned long t = millis();
  //state = State{"", 0, t, t, 0.0f, 0.0f, &SampleMode};
}


void loop() {
   long irValue = particleSensor.getIR();
   unsigned long t = millis();

  // if (irValue > 5000 && checkForBeat(irValue) == true) {
  //   //We sensed a beat!
  //   long delta = t - lastBeat;
  //   lastBeat = t;

  //   beatsPerMinute = 60 / (delta / 1000.0);

  //   if (beatsPerMinute < 255 && beatsPerMinute > 20)
  //   {
  //     rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
  //     rateSpot %= RATE_SIZE; //Wrap variable

  //     //Take average of readings
  //     beatAvg = 0;
  //     for (byte x = 0 ; x < RATE_SIZE ; x++)
  //       beatAvg += rates[x];
  //     beatAvg /= RATE_SIZE;
  //   }
  // }

  if (irValue > 5000 && checkForBeat(irValue) == true) {
    Serial.print("pulse: ");
    beatsPerMinute++;
    Serial.println(beatsPerMinute);
  }

  if ((t - lastPrint) > 15000) {
    Serial.print("BPM: ");
    Serial.println(beatsPerMinute * 4);
    lastPrint = t;
    beatsPerMinute = 0;

  }

  //delay(100);
}