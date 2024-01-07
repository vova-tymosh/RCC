/* 
 *   
 *   
 *   
 */
#include <Arduino.h>
#include <Wire.h>
#include "Intercom.h"

Intercom intercom;

void receiveEvent(int bytes);
void requestEvent();
    
void Intercom::setup(bool master) {
  this->master = master;
  if (master) {
    Wire.begin();
  } else {
    Wire.begin(I2C_CODE);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
  }
}

void Intercom::send(data_t* data, uint8_t size) {
  if (size > PACKET_LEN)
    size = PACKET_LEN;
  if (this->master) {
    Wire.beginTransmission(I2C_CODE);
    Wire.write(size);
    Wire.write((uint8_t*)data, size * sizeof(data_t));
    Wire.endTransmission();
  } else {
    this->size = size;
    memcpy(packetOut, data, size * sizeof(data_t));
  }
}
int Intercom::internalRecv() {
  uint8_t size = 0;
  if (Wire.available()) {
    size = Wire.read();
  }
//  Serial.println("internalRecv:" + String(size));

  if (size > PACKET_LEN)
    size = PACKET_LEN;
  uint8_t *packetBytes = (uint8_t*)packetIn;
  for (int i = 0; i < size  * sizeof(data_t); i++) {
    if (Wire.available())
      packetBytes[i] = Wire.read();
  }
  return size;
}

void Intercom::recv(data_t* data, uint8_t size) {
  if (this->master) {
    Wire.requestFrom(1, PACKET_LEN + 1);
    uint8_t size = internalRecv();
  }
  memcpy(data, packetIn, size * sizeof(data_t));
}

void Intercom::onReceive(int bytes) {
  internalRecv();
}
void Intercom::onSend() {
//  Serial.println("send:" + String(packetOut[0]) + "/" + String(packetOut[1]) + "/" + size);
  Wire.write(size);
  Wire.write((const uint8_t*)packetOut, size*sizeof(data_t));
}

void receiveEvent(int bytes) {
  intercom.onReceive(bytes);
}
void requestEvent() {
  intercom.onSend();
}
