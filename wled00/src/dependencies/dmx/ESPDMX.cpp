// - - - - -
// ESPDMX - A Arduino library for sending and receiving DMX using the builtin serial hardware port.
// ESPDMX.cpp: Library implementation file
//
// Copyright (C) 2015  Rick <ricardogg95@gmail.com>
// This work is licensed under a GNU style license.
//
// Last change: Marcel Seerig <https://github.com/mseerig>
//
// Documentation and samples are available at https://github.com/Rickgg/ESP-Dmx
// - - - - -

/* ----- LIBRARIES ----- */
#ifdef ESP8266

#include <Arduino.h>

#include "ESPDMX.h"



#define dmxMaxChannel  512
#define defaultMax 32

#define DMXSPEED       250000
#define DMXFORMAT      SERIAL_8N2
#define BREAKSPEED     83333
#define BREAKFORMAT    SERIAL_8N1

bool dmxStarted = false;
int sendPin = 16;		// DREW - This is the Serial2 TX Pin
int recievePin = 5;

//DMX value array and size. Entry 0 will hold startbyte
uint8_t dmxDataStore[dmxMaxChannel] = {};
int channelSize;


void DMXESPSerial::init() {
  channelSize = defaultMax;

  pinMode(sendPin, OUTPUT);
  Serial2.begin(DMXSPEED, DMXFORMAT, recievePin, sendPin);
  dmxStarted = true;
}

// Set up the DMX-Protocol
void DMXESPSerial::init(int chanQuant) {

  if (chanQuant > dmxMaxChannel || chanQuant <= 0) {
    chanQuant = defaultMax;
  }

  channelSize = chanQuant;

  pinMode(sendPin, OUTPUT);
  Serial2.begin(DMXSPEED, DMXFORMAT, recievePin, sendPin);
  dmxStarted = true;
}

// Function to read DMX data
uint8_t DMXESPSerial::read(int Channel) {
  if (dmxStarted == false) init();

  if (Channel < 1) Channel = 1;
  if (Channel > dmxMaxChannel) Channel = dmxMaxChannel;
  return(dmxDataStore[Channel]);
}

// Function to send DMX data
void DMXESPSerial::write(int Channel, uint8_t value) {
  if (dmxStarted == false) init();

  if (Channel < 1) Channel = 1;
  if (Channel > channelSize) Channel = channelSize;
  if (value < 0) value = 0;
  if (value > 255) value = 255;

  dmxDataStore[Channel] = value;
}

void DMXESPSerial::end() {
  chanSize = 0;
  Serial2.end();
  dmxStarted = false;
}

void DMXESPSerial::update() {
  if (dmxStarted == false) init();

  //Send break
  digitalWrite(sendPin, HIGH);
  Serial2.begin(BREAKSPEED, BREAKFORMAT, recievePin, sendPin);
  Serial2.write(0);
  Serial2.flush();
  delay(1);
  Serial2.end();

  //send data
  Serial2.begin(DMXSPEED, DMXFORMAT, recievePin, sendPin);
  digitalWrite(sendPin, LOW);
  Serial2.write(dmxData, chanSize);
  Serial2.flush();
  delay(1);
  Serial2.end();
}

// Function to update the DMX bus

#endif