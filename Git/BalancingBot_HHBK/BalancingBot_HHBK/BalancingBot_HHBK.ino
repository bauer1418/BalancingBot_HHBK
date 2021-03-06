/*
 Name:		BalancingBot_HHBK.ino
 Created:	19.07.2017 02:36:37
 Author:	Andreas Bauer
*/

//Ben�tigte Bibilotheken

#include <EEPROMVar.h>
#include <EEPROMex.h>
#include <Adafruit_NeoPixel.h>
#include <CmdMessenger.h>
#include "Andi_Stepper_Motor.h"
#include "Andi_Bibilothek_BalancingBot.h"
#include <PID_v1_Andi.h>

unsigned long Zykluszeit=0;				//akutelle Zykluszeit
bool MotorenEINAUS = false;				//Motoren Ein Aus Schalter 


double Sollwert_PID_Winkel, Eingang_PID_Winkel, Ausgang_PID_Winkel;//PID Regler Werte
double Sollwert_PID_Geschwindigkeit, Eingang_PID_Geschwindigkeit, Ausgang_PID_Geschwindigkeit;//PID Regler Werte f�r Geschwindigkeitsregler


PID PID_Regler_Winkel(&Eingang_PID_Winkel, &Ausgang_PID_Winkel, &Sollwert_PID_Winkel, 10,0,0,DIRECT);//PID-Regler f�r Wickelsteuerung
PID PID_Regler_Geschwindigkeit(&Eingang_PID_Geschwindigkeit,&Ausgang_PID_Geschwindigkeit,&Sollwert_PID_Geschwindigkeit,1,1,1,DIRECT);

CmdMessenger cmdMessenger = CmdMessenger(Serial);

 //the setup function runs once when you press reset or power the board
void setup() 
{
	Pin_Setup();

	analogWrite(Pin_Platinenluefter,207);//Platinenl�fter auf 80% stellen

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
	PID_Regler_Winkel.SetOutputLimits(-120,120);//!!!!!!!!!!!!!!!!!!!!!!!!!!HIER IST DER FEHLER MIT PID AUSGANG NUR 0-255!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Sollwert_PID_Winkel=0.00;
	Motor_Links.StepMode_setzen(Stepper_Motor::Vollschritt);
	Motor_Rechts.StepMode_setzen(Stepper_Motor::Vollschritt);
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
		//Eingang_PID_Winkel=GET_KalmanWinkelX();
		//PID-Regler ausf�hren
		//PID_Regler_Winkel.Compute();//PID-Regler f�r die Winkelsteuerung zyklisch ausf�hren
		//Motoren_Steuerung(Ausgang_PID_Winkel,100,100);
		
		Ausgangsregister_schreiben(Motor_Links.Step(1,100),false);
	}

	//Umkippschutz(20,GET_KalmanWinkelX());

	//PID_Regler_Geschwindigkeit.Compute();
	Akkuueberwachung(6,7);
	//Lueftersteuerung_Temperatur(GET_MPU_Temperatur(),Pin_Gehaeuseluefter);//Geh�usel�ftersteuerung
	//cmdMessenger.sendCmd(cmd_Anzeige_Text,analogRead(Akkuspannung2));

	cmdMessenger.feedinSerialData();//cmdMessenger Datenauslesen und Callbacks ausl�sen
}
