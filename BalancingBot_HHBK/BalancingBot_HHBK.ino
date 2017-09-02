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
int Status = 0;		//Systemstatus Variable Bedeutung siehe Enum Statusmeldungen
double Akkuspannung1 = 0.00;	//Akkuspannung 1 aus der Akkuüberwachung
double Akkuspannung2 = 0.00;	//Akkuspannung 2 aus der Akkuüberwachung
double AkkuSpannungMin = 6.00;	//AkkuEntladespannung 0%
double AkkuSpannungMax = 8.40;	//Akkuspannung bei Vollgeladenem Akku 100%
double Akku_Prozent = 0.0;		//Akkustand in Prozent

CmdMessenger cmdMessenger = CmdMessenger(Serial);



 //the setup function runs once when you press reset or power the board
void setup() 
{
	Setup_cmdMessenger();
	Serial.begin(115200);
	Pin_Setup();
	
	Status=Setup_beendet;
	Statusmeldung();

}

 //the loop function runs over and over again until power down or reset
void loop() 
{
	//Akkuspannungen ausrechnen
	//Kalmanfilter ausführen
	//PID-Regler ausführen
	//Motoren einstellen
  cmdMessenger.feedinSerialData();//cmdMessenger Datenauslesen und Callbacks auslösen
}
