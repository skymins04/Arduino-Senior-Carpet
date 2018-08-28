/*
 Name:		senior_Carpet.ino
 Created:	2018-07-21 오후 6:07:22
 Author:	BetaMan
*/

#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include "Notes.h"

uint8_t DEBUG = 0;

uint8_t BZ = 5;
uint8_t R = 11;
uint8_t G = 10;
uint8_t B = 9;
uint16_t L_FSR = 0;
uint16_t R_FSR = 1;
String Level[4] = { "None", "Good", "so so", "bed" };
uint8_t status = 0;
int detected = 0;
long elapsedTime = 0;
long tempTime = 0;
long temp = 0;
boolean startFlag = true;
boolean tempFlag = false;
boolean MSGFlag = true;

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial bt(2, 3); // RX, TX

void printLCD(uint8_t col, uint8_t row, String str) {
	lcd.setCursor(col, row);
	lcd.print(str);
}

void startScreen() {
	lcd.clear();
	printLCD(0, 0, "=Senior  Safety=");
	printLCD(0, 1, "---Alert Sys.---");

	digitalWrite(G, HIGH); digitalWrite(B, HIGH);
	tone(BZ, 784, 125);	delay(125);
	tone(BZ, 1047, 125); delay(125);
	tone(BZ, 1175, 125); delay(125);
	tone(BZ, 880, 125); delay(125);
	tone(BZ, 1175, 125); delay(125);
	tone(BZ, 1319, 125); delay(125);
	tone(BZ, 1397, 1000); delay(1000);
	delay(1000);
	lcd.clear();
	digitalWrite(G, LOW); digitalWrite(B, LOW);
}

void setup() {
	if (DEBUG == 1) Serial.begin(9600);
	bt.begin(9600);
	lcd.begin();

	pinMode(BZ, OUTPUT); digitalWrite(BZ, LOW);
	pinMode(R, OUTPUT); digitalWrite(R, LOW);
	pinMode(G, OUTPUT); digitalWrite(G, LOW);
	pinMode(B, OUTPUT); digitalWrite(B, LOW);

	startScreen();
}

void loop() {
			while (1) {
				printLCD(0, 0, "Activity:"); printLCD(10, 0, "      "); printLCD(10, 0, Level[status]);
				if (DEBUG == 1) { printLCD(0, 1, "detect:"); printLCD(8, 1, String(detected)); }
				
				if (status == 1) {
					digitalWrite(R, LOW);
					digitalWrite(G, HIGH);
					digitalWrite(B, LOW);
				}
				else if (status == 2) {
					analogWrite(R, 200);
					analogWrite(G, 50);
					analogWrite(B, 0);
				}
				else if(status == 3){
					digitalWrite(R, HIGH);
					digitalWrite(G, LOW);
					digitalWrite(B, LOW);
					bt.println("=| Warning message |====================\nThe activity of the elderly is too little.\nYou should visit elder\'s house.\n========================================\n");
					while (!(analogRead(L_FSR) > 300) || (analogRead(R_FSR) > 300)) {
						if (tempFlag) {
							tone(BZ, G5);
							tempFlag = false;
						}
						else {
							tone(BZ, E5);
							tempFlag = true;
						}
						delay(300);
					}
					noTone(BZ);
					detected = temp+2;
				}

				if ((analogRead(L_FSR) > 300) || (analogRead(R_FSR) > 300)) {
					detected += 1;
					if (startFlag) { 
						startFlag = false;
						elapsedTime = millis();
					}
					tempTime = millis();
					while ((analogRead(L_FSR) > 300) || (analogRead(R_FSR) > 300)) {
						if (millis() - tempTime > 10000) {
							if (MSGFlag) {
								bt.println("=| Warning message |====================\nIt has been detected for too long.\nYou should visit elder\'s house.\n========================================\n");
								digitalWrite(R, HIGH);
								digitalWrite(G, LOW);
								digitalWrite(B, LOW);
								MSGFlag = false;
							}
							printLCD(10, 0, "      ");  printLCD(10, 0, Level[3]);
							if (tempFlag) {
								tone(BZ, G5);
								tempFlag = false;
							}
							else {
								tone(BZ, E5);
								tempFlag = true;
							}
							delay(300);
						}
					}
					noTone(BZ);
					MSGFlag = true;
				}
				if (detected != 0) {
					temp = (millis() - elapsedTime) / 10000; if (temp < 0) temp * -1;
					if (DEBUG == 1) { Serial.println(String(millis()) + " - " + String(elapsedTime) + " / " + String(detected) + " = " + String(temp)); }
					
					if (temp <= detected) status = 1;
					else if (temp-1 <= detected) status = 2;
					else status = 3;
				}
			}
}