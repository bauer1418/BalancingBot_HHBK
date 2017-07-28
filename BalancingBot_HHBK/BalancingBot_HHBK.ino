/*
 Name:		BalancingBot_HHBK.ino
 Created:	19.07.2017 02:36:37
 Author:	Andreas Bauer
*/

//Benötigte Bibilotheken

#include <CmdMessenger.h>
#include "Andi_Bibilothek_BalancingBot.h"
#include <Wire.h>
#include <MPU6050.h>
#include <I2Cdev.h>
#include <helper_3dmath.h>
#include <Kalman.h>
#include <EEPROM.h>
#include <PID_v1_Andi.h>

//Variablen




CmdMessenger cmdMessenger = CmdMessenger(Serial);

enum Befehle
{
	
	cmd_Statusmeldung,			//Statusmeldungen z.B.: Setup beendet
	cmd_Akkustand,				//Aktuelle Spannung des Primärakkus
	cmd_KalmanWinkel,			//Aktueller Winkel aus dem Kalman Filter
	cmd_RAW_Werte,				//RAW-Werte aus dem MPU ohne Bearbeitung
	cmd_Offset_Werte,			//Offset-Werte für die XYZ Achsen des MPU6050
	cmd_MPU_Kalibrieren,		//Automatik Kalibrierung durchführen
	
	cmd_PID_Winkel_MinMax,		//Winkel Regler Min und Max Werte setzen
	cmd_PID_Winkel_Sollwert,		//Winkel Regler Sollwert setzen
};

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

  cmdMessenger.feedinSerialData();//cmdMessenger Datenauslesen und Callbacks auslösen
}
