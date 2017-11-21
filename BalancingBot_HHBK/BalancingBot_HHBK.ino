/*
 Name:		BalancingBot_HHBK.ino
 Created:	19.07.2017 02:36:37
 Author:	Andreas Bauer
*/


unsigned long Zykluszeit=0;				//akutelle Zykluszeit
bool MotorenEINAUS = false;				//Motoren Ein Aus Schalter 

//Ben�tigte Bibilotheken
#include <Adafruit_NeoPixel.h>
#include "Andi_Bibilothek_BalancingBot.h"
#include "MPU6050_BalancingBot_HHBK.h"
#include "Messenger_Befehle.h"
#include "Andi_ZeitTakt_Funktionen.h"






 //the setup function runs once when you press reset or power the board
void setup() 
{
	Pin_Setup();

	Setup_cmdMessenger();

	NeoPixel_Setup(25);

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
	PID_Regler_Winkel.SetSampleTime(10);
	PID_Regler_Winkel.SetOutputLimits(-120,120);//!!!!!!!!!!!!!!!!!!!!!!!!!!HIER IST DER FEHLER MIT PID AUSGANG NUR 0-255!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Sollwert_PID_Winkel=0.00;
	Motor_Links.StepMode_setzen(Stepper_Motor::Halbschritt);
	Motor_Rechts.StepMode_setzen(Stepper_Motor::Halbschritt);
	Statusmeldung();
}

 //the loop function runs over and over again until power down or reset
void loop() 
{
	Zykluszeit = Zykluszeit_Messung();
	Allgemeine_Zeitfunktion.ZeitTakt();
	
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
		analogWrite(Pin_Platinenluefter,80*255/100);

		//Winkel auslesen
		Eingang_PID_Winkel=GET_KalmanWinkelY();
		if (Eingang_PID_Winkel>2 || Eingang_PID_Winkel<-2)
		{
			PID_Regler_Winkel.SetTunings(System_Einstellungen.PID_Regler_Winkel_Fern_P, System_Einstellungen.PID_Regler_Winkel_Fern_I, System_Einstellungen.PID_Regler_Winkel_Fern_D);
		/*	Motor_Links.StepMode_setzen(Stepper_Motor::Halbschritt);
			Motor_Rechts.StepMode_setzen(Stepper_Motor::Halbschritt);
		*/}
		else
		{
			PID_Regler_Winkel.SetTunings(System_Einstellungen.PID_Regler_Winkel_Nah_P, System_Einstellungen.PID_Regler_Winkel_Nah_I, System_Einstellungen.PID_Regler_Winkel_Nah_D);
		/*	Motor_Links.StepMode_setzen(Stepper_Motor::Halbschritt);
			Motor_Rechts.StepMode_setzen(Stepper_Motor::Halbschritt);
		*/}
		//PID-Regler ausf�hren
		PID_Regler_Winkel.Compute();//PID-Regler f�r die Winkelsteuerung zyklisch ausf�hren
		Motoren_Steuerung(Ausgang_PID_Winkel,100,100);
	}
	else
	{
		analogWrite(Pin_Platinenluefter,0);
	}
		//Ausgangsregister_schreiben(Motor_Links.Step(100,100),Motor_Rechts.Step(100,100));
//}


	if (Umkippschutz(40,GET_KalmanWinkelY())==true)
	{
		MotorenEINAUS=false;
		Motoren_EINAUS_Schalten(MotorenEINAUS);
	}

	//PID_Regler_Geschwindigkeit.Compute();
	Akkuueberwachung(6,7);
	//Lueftersteuerung_Temperatur(GET_MPU_Temperatur(),Pin_Gehaeuseluefter);//Geh�usel�ftersteuerung
	//cmdMessenger.sendCmd(cmd_Anzeige_Text,analogRead(Akkuspannung2));

	cmdMessenger.feedinSerialData();//cmdMessenger Datenauslesen und Callbacks ausl�sen
}
