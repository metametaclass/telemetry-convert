/*
 * Telemetry converter
 *
 * Licensed under GPLv3
 * S.Port communication code taken from openxvario project
 *
 * Author: Jochen Tuchbreiter
*/


// Serial interfaces used
#define _debugSerial Serial

#define _sportSerial Serial1
#define _sport_C1 UART0_C1
#define _sport_C3 UART0_C3
#define _sport_S2 UART0_S2

#define _inputSerial Serial2
#define _input_C1 UART1_C1
#define _input_C3 UART1_C3


// Frsky Sensor-ID to use. 
#define SENSOR_ID 0xA1 // ID of sensor. Must be something that is polled by FrSky RX


// Frsky-specific
#define START_STOP         0x7e
#define DATA_FRAME         0x10


// FrSky Value-IDs
#define FR_ID_SPEED 0x0830 
#define FR_ID_VOLTAGE 0x0210 
#define FR_ID_CURRENT 0x0200 
#define FR_ID_RPM 0x0500
#define FR_ID_CAP_USED 0x0600
#define FR_ID_ALTITUDE 0x0100
#define FR_ID_VARIO 0x0110


// Sensor-Values transmitted to S.BUS
#define NUM_VALUES 7

#define SPEED 0
#define VOLTAGE 1
#define CURRENT 2
#define RPM 3
#define CAP_USED 4
#define ALTITUDE 5
#define VARIO 6

byte valueChange[NUM_VALUES];

uint32_t speed = 0; // 1 km/h
uint32_t voltage = 0; // 0.01V
uint32_t current = 0; // 0.01A
uint32_t rpm = 0; // 1/2 RPM
uint32_t cap_used = 0; // 1mah
int32_t altitude = 0; // 1m
int32_t vario = 0; // 0.01m


short crc; // used for crc calc of frsky-packet
uint8_t lastRx = 0;

uint16_t currValue = 0;

void sendByte(uint8_t byte) {
	_sportSerial.write(byte);
	// CRC update
	crc += byte; //0-1FF
	crc += crc >> 8; //0-100
	crc &= 0x00ff;
	crc += crc >> 8; //0-0FF
	crc &= 0x00ff;
}

void sendCrc() {
	_sportSerial.write(0xFF-crc);
	// CRC reset
	crc = 0;
}

void sendValue(uint8_t header, uint16_t id, uint32_t value) {
	_sport_C3 |= 32;
	sendByte(header);
	uint8_t *bytes = (uint8_t*)&id;
	sendByte(bytes[0]);
	sendByte(bytes[1]);
	bytes = (uint8_t*)&value;
	sendByte(bytes[0]);
	sendByte(bytes[1]);
	sendByte(bytes[2]);
	sendByte(bytes[3]);
	sendCrc();
	Serial1.flush();
	_sport_C3 ^= 32;
}

void decodeMLink(byte data1, byte data2, byte data3) {
	#define ALERT_MASK 0x01
	
	uint8_t address = 0;
	uint8_t dataType = 0;
	int16_t value = 0;
	boolean alert=false;
	
	/*_debugSerial.print("Decoding: ");
	_debugSerial.print(data1, HEX);
	_debugSerial.print(" ");
	_debugSerial.print(data2, HEX);
	_debugSerial.print(" ");
	_debugSerial.print(data3, HEX);
	_debugSerial.print("  ");
	_debugSerial.println();*/
	if (data2 & ALERT_MASK) {
		alert = true;
	}
	dataType = data1 & 0x0F;
	address = data1 >> 4;
	
	value = data2;
	value |= data3 << 8;
	value = value >> 1;
	
	/*
	  _debugSerial.print("address: ");
	_debugSerial.print(address);
	_debugSerial.print(" dataType: ");
	_debugSerial.print(dataType);
	_debugSerial.print(" value: ");
	_debugSerial.print(value);
	_debugSerial.print(" ");
	_debugSerial.print(value, HEX);
	_debugSerial.print(" alert: ");
	_debugSerial.print(alert);*/
	
	/*  _debugSerial.print("Got address: ");
	_debugSerial.println(address);*/
	
	
	if (dataType == 1) { // voltage 0.1V
			voltage = value*10;
			_debugSerial.print("Got voltage: ");
			_debugSerial.print(value / 10);
			_debugSerial.print(".");
			_debugSerial.print(value % 10);
			_debugSerial.println();
		} else if (dataType == 2) { // current 0.1A
			current = value;
			_debugSerial.print("  Got current: ");
			_debugSerial	.print(value / 10);
			_debugSerial.print(".");
			_debugSerial.print(value % 10);
			_debugSerial.println();
		} else if (dataType == 3) { // vario 0.1m/s
			vario = value*10;
			_debugSerial.print("    Got vario: ");
			_debugSerial.print(value);
			/*_debugSerial.print(".");
			_debugSerial.print(value % 10);*/
			_debugSerial.println();
		} else if (dataType == 4) { // speed 0.1km/h
			speed=value/10;
			_debugSerial.print("      Got speed: ");
			_debugSerial.print(value / 10);
			_debugSerial.print(".");
			_debugSerial.print(value % 10);
			_debugSerial.println();
		} else if (dataType == 5) { // revs -10 1/min
			value=-value;
			rpm = value*20; // Assume 2 blades configured in Taranis
			_debugSerial.print("      Got rpm: ");
			_debugSerial.print(value*10);
			_debugSerial.println();
			
		} else if (dataType == 6) { // temp 0.1C
		} else if (dataType == 7) { // direction 0.1 degree
		} else if (dataType == 8) { // altitude 1m
			altitude= value*100;
			_debugSerial.print("          Got alt: ");
			_debugSerial.print(altitude);
			_debugSerial.println();
		} else if (dataType == 9) { // tank level  1%
		} else if (dataType == 10) { // link quality 1%
		} else if (dataType == 11) { // charge used 1mah
			cap_used= value;
			_debugSerial.print("          Got mah: ");
			_debugSerial.print(value);
			_debugSerial.println();
			
		} else if (dataType == 12) { // fluid 1ml
		} else if (dataType == 13) { // long distance 0.1km
	}
	
	
	// Sending value 0x8000 means: No value delivered by sensor (keeps last value)
	
}

void processSPort() {
	uint8_t data = 0;
	while (_sportSerial.available()) {
		data = _sportSerial.read();
		if (lastRx == START_STOP && data == SENSOR_ID) {
			
			if (currValue == SPEED) {
			  sendValue(DATA_FRAME, FR_ID_SPEED, speed);	
			} else if (currValue == VOLTAGE) {
			  sendValue(DATA_FRAME, FR_ID_VOLTAGE, voltage);
			} else if (currValue == CURRENT) {
			  sendValue(DATA_FRAME, FR_ID_CURRENT, current);
			} else if (currValue == RPM) {
				sendValue(DATA_FRAME, FR_ID_RPM, rpm);
			} else if (currValue == CAP_USED) {
				sendValue(DATA_FRAME, FR_ID_CAP_USED, cap_used);
			} else if (currValue == ALTITUDE) {
				sendValue(DATA_FRAME, FR_ID_ALTITUDE, altitude);
			} else if (currValue == VARIO) {
				sendValue(DATA_FRAME, FR_ID_VARIO, vario);
			}
			currValue += 1;
			currValue = currValue % NUM_VALUES;
		}
		lastRx = data;
	}
}



void setup() {
	_debugSerial.begin(38400);
	_inputSerial.begin(38400);
	_sportSerial.begin(57600);
	_sport_C3 |= 16; // TX invert
	_sport_C1 |= 32+128; // Single wire mode
	_sport_S2 |= 16; // RX invert
	
	_input_C1 |= 32+128; // Single wire mode
}


void loop() {
	byte data1;
	byte data2;
	byte data3;
	
	while (1) {
		for (byte i=1;i<16;i++) {
			while(_inputSerial.available() > 0) { _inputSerial.read(); } // throw away any received bytes
			_input_C3 |= 32;		    
			_inputSerial.write(i); // send sensor address
			_inputSerial.flush(); // make sure send is complete
  			_input_C3 ^= 32;
			processSPort();
			delay(6); // wait up to 6ms for answer
			if (_inputSerial.available() == 3) { // 3 bytes received means: got valid reply
				data1 = _inputSerial.read();
				data2 = _inputSerial.read();
				data3 = _inputSerial.read();
				decodeMLink(data1, data2, data3);
			}
		}
	}
}



