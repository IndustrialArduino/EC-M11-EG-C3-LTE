/**
   Multiple Serial test

   Receives from the main serial port, sends to the others.
   Receives from serial port 1, sends to the main serial (Serial 0).

   This example works only with boards with more than one serial like Arduino Mega, Due, Zero, etc.

   The circuit:
   - any serial device attached to Serial port 1
   - Serial Monitor open on Serial port 0

   created 30 Dec 2008
   modified 20 May 2012
   by Tom Igoe & Jed Roach
   modified 27 Nov 2015
   by Arturo Guadalupi

   This example code is in the public domain.
**/
#include "HX711.h"
#define MODEM_RESET 32
#define MODEM_FLIGHT 22
#define MODEM_RX 26
#define MODEM_TX 25

const int LOADCELL_DOUT_PIN = 33;
const int LOADCELL_SCK_PIN = 32;

#define RS485_RX 4
#define RS485_TX 2
#define RS485_FC 13

HX711 scale;

long timer1;

void setup() {
  Serial.begin(115200);

  // Modem initialization
  pinMode(MODEM_FLIGHT, OUTPUT); // FLIGHT MODE ENABLE
  pinMode(MODEM_RESET, OUTPUT);  // MODEM RESET PIN
  digitalWrite(MODEM_FLIGHT, HIGH); // FLIGHT MODE
  MODEM_RESET_CYC();
  delay(2000);
  Serial2.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  Serial.println("SIM AT ATART >>>>>>>>>>>>>>");
  delay(2000);
  Serial2.println("AT");
  delay(2000);
  Serial2.println("AT+CPIN?");
  delay(2000);
  Serial2.println("AT+CNMP?");

  // Scale initialization
  pinMode(RS485_FC, OUTPUT);
  digitalWrite(RS485_FC, HIGH); // RS-485
  Serial1.begin(9600, SERIAL_8N1, RS485_RX, RS485_TX);
  Serial1.println("RS-485 OK >>>> ");
  delay(500);
  Serial.println("HX711 Demo");
  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());

  scale.set_scale(2280.f);
  scale.tare();
  Serial.println("After setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());
  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));
  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));
  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);
  Serial.println("Readings:");
}

void loop() {
  // Modem part
  delay(3000);
  timer1 = millis();
  Serial2.println("AT");
  while (millis() < timer1 + 10000) {
    while (Serial2.available()) {
      int inByte = Serial2.read();
      Serial.write(inByte);
    }
  }

  timer1 = millis();
  Serial2.println("AT+CPIN?");
  while (millis() < timer1 + 10000) {
    while (Serial2.available()) {
      int inByte = Serial2.read();
      Serial.write(inByte);
    }
  }

  // Scale part
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 1);

  scale.power_down();

  /***********************************/
  digitalWrite(RS485_FC, HIGH);
  delay(100);
  Serial1.println("RS-485 OK >>>> ");
  delay(100);
  digitalWrite(RS485_FC, LOW);

  while (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
  }
  /***********************************/

  scale.power_up();
}

void MODEM_RESET_CYC() {
  digitalWrite(MODEM_RESET, HIGH);
  delay(1000);
  digitalWrite(MODEM_RESET, LOW);
  delay(1000);
  digitalWrite(MODEM_RESET, HIGH);
}
