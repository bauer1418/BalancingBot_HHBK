/*
 Name:		BalancingBot_HHBK.ino
 Created:	19.07.2017 02:36:37
 Author:	Andreas Bauer
*/

#if defined(ARDUINO) && ARDUINO >= 100

#include "arduino.h"
#else
	#include "WProgram.h"
#endif



//Benötigte Bibilotheken

//
#include "Messenger_Enum.h"
#include "Messenger_Befehle.h"
#include "NeoPixel.h"
#include "Andi_Bibilothek_BalancingBot.h"
#include "MPU6050_BalancingBot_HHBK.h"
#include "EEPROM_Funktionen.h"
#include "TEST.h"




 //the setup function runs once when you press reset or power the board
void setup() 
{
	System_Einstellungen=Daten_aus_EEPROM_lesen();

	
	if (Daten_aus_EEPROM_lesen().Werte_ueberschrieben=0)
	{
		Einstellungen_mit_Standart_Werten_beschreiben();
	}

	EEPROM_Werte_aktiveren(System_Einstellungen);
	
	Pin_Setup();

	Setup_cmdMessenger();


	if (MPUsetup==false)
	{
		Status=Fehler;
	}
	else
	{
		Status=Setup_beendet;
	}
	PID_Regler_Winkel.SetControllerDirection(DIRECT);
	PID_Regler_Winkel.SetMode(AUTOMATIC);
	//PID_Regler_Winkel.SetOutputLimits(System_Einstellungen.PID_Regler_Winkel_Min,System_Einstellungen.PID_Regler_Winkel_Max);//!!!!!!!!!!!!!!!!!!!!!!!!!!HIER IST DER FEHLER MIT PID AUSGANG NUR 0-255!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Sollwert_PID_Winkel=0.00;
	//Motor_Links.StepMode_setzen(System_Einstellungen.Schrittmodus);
	//Motor_Rechts.StepMode_setzen(System_Einstellungen.Schrittmodus);
	
	Statusmeldung();
}

 //the loop function runs over and over again until power down or reset
void loop() 
{
	Zykluszeit = Zykluszeit_Messung();
	Zeit_Takt_20ms();
	
	//Akkuueberwachung(Pin_Akku1_Messung,Pin_Akku2_Messung);
	//MPU Zyklus nur ausf�hren wenn MPU nicht gest�rt ist
	if (Fehlerspeicher!=MPU_NOT_FOUND && Fehlerspeicher!=MPU_READ_FAILED && Fehlerspeicher!=MPU_READ_TIMEOUT && Fehlerspeicher!=MPU_Write_FAILED)
	{
		MPU_Zyklus();
	}
	//else
	//{
	//	Fehlerspeicher=Kein_Fehler_vorhanden;
	//}
	Zyklusdaten_senden();

	if (MotorenEINAUS==true)
	{
		//Winkel auslesen
		Eingang_PID_Winkel=GET_KalmanWinkelY();
		//PID-Regler ausf�hren
		PID_Regler_Winkel.Compute();//PID-Regler f�r die Winkelsteuerung zyklisch ausf�hren
		Motoren_Steuerung(Ausgang_PID_Winkel,100,100);
		
		//Ausgangsregister_schreiben(Motor_Links.Step(1,0),Motor_Rechts.Step(1,0));
	}

	//Umkippschutz(20,GET_KalmanWinkelY());

	//PID_Regler_Geschwindigkeit.Compute();
	Akkuueberwachung(6,7);
	//Lueftersteuerung_Temperatur(GET_MPU_Temperatur(),Pin_Gehaeuseluefter);//Geh�usel�ftersteuerung
	//cmdMessenger.sendCmd(cmd_Anzeige_Text,analogRead(Akkuspannung2));

	cmdMessenger.feedinSerialData();//cmdMessenger Datenauslesen und Callbacks ausl�sen
}

