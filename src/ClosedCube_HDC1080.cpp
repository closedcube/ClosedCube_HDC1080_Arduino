/*

Arduino Library for Texas Instruments HDC1080 Digital Humidity and Temperature Sensor
Written by AA for ClosedCube
---

The MIT License (MIT)

Copyright (c) 2016-2017 ClosedCube Limited

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include <Wire.h>

#include "ClosedCube_HDC1080.h"


ClosedCube_HDC1080::ClosedCube_HDC1080()
{
}

void ClosedCube_HDC1080::begin(uint8_t address) {
	_address = address;
	Wire.begin();

        // Heater off, 14 bit Temperature and Humidity Measurement Resolution 
	Wire.beginTransmission(_address);
	Wire.write(CONFIGURATION);
	Wire.write(0x0);
	Wire.write(0x0);
	Wire.endTransmission();

}

HDC1080_Registers ClosedCube_HDC1080::readRegister() {
	HDC1080_Registers reg;
	reg.rawData = (readData(CONFIGURATION) >> 8);
	return reg;
}

void ClosedCube_HDC1080::writeRegister(HDC1080_Registers reg) {
	Wire.beginTransmission(_address);
	Wire.write(CONFIGURATION);
	Wire.write(reg.rawData);
	Wire.write(0x00);
	Wire.endTransmission();
	delay(10);
}

void ClosedCube_HDC1080::heatUp(uint8_t seconds) {
	HDC1080_Registers reg = readRegister();
	reg.Heater = 1;
	reg.ModeOfAcquisition = 1;
	writeRegister(reg);

	uint8_t buf[4];
	for (int i = 1; i < (seconds*66); i++) {
		Wire.beginTransmission(_address);
		Wire.write(0x00);
		Wire.endTransmission();
		delay(20);
		Wire.requestFrom(_address, (uint8_t)4);
		Wire.readBytes(buf, (size_t)4);
	}
	reg.Heater = 0;
	reg.ModeOfAcquisition = 0;
	writeRegister(reg);
}


float ClosedCube_HDC1080::readT() {
	return readTemperature();
}

float ClosedCube_HDC1080::readTemperature() {
	uint16_t rawT = readData(TEMPERATURE);
	return ((rawT / pow(2, 16)) * 165) - 40;
}

float ClosedCube_HDC1080::readH() {
	return readHumidity();
}

float ClosedCube_HDC1080::readHumidity() {
	uint16_t rawH = readData(HUMIDITY);
	return (rawH / pow(2, 16)) * 100;
}

uint16_t ClosedCube_HDC1080::readManufacturerId() {
	return readData(MANUFACTURER_ID);
}

uint16_t ClosedCube_HDC1080::readDeviceId() {
	return readData(DEVICE_ID);
}

uint16_t ClosedCube_HDC1080::readData(uint8_t pointer) {
	Wire.beginTransmission(_address);
	Wire.write(pointer);
	Wire.endTransmission();
	
	delay(9);
	Wire.requestFrom(_address, (uint8_t)2);

	byte msb = Wire.read();
	byte lsb = Wire.read();

	return msb << 8 | lsb;
}



