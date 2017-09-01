/*
 Name:		BalancingBot_HHBK.ino
 Created:	19.07.2017 02:36:37
 Author:	Andreas Bauer
*/

//Benötigte Bibilotheken

#include <Adafruit_NeoPixel.h>
#include <CmdMessenger.h>
#include "Andi_Bibilothek_BalancingBot.h"
#include <Wire.h>
#include <MPU6050.h>
#include <I2Cdev.h>
#include <helper_3dmath.h>
#include <Kalman.h>
#include <EEPROM.h>
#include <PID_v1_Andi.h>
#include "Messenger_Enum.h"

//Variablen




CmdMessenger cmdMessenger = CmdMessenger(Serial);



 //the setup function runs once when you press reset or power the board
void setup() 
{
	Setup_cmdMessenger();
	Serial.begin(115200);
	cmdMessenger.printLfCr();//Nach jeder Message eine neue Zeile beginnen
	cmdMessenger.sendCmd(cmd_Statusmeldung, (F("Setup beendet!")));
}

 //the loop function runs over and over again until power down or reset
void loop() 
{
	//Kalmanfilter ausführen
	//PID-Regler ausführen
	//Motoren einstellen
  cmdMessenger.feedinSerialData();//cmdMessenger Datenauslesen und Callbacks auslösen
}
