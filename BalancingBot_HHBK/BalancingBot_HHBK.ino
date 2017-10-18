/*
 Name:		BalancingBot_HHBK.ino
 Created:	19.07.2017 02:36:37
 Author:	Andreas Bauer
*/

//Benötigte Bibilotheken

#include <EEPROMVar.h>
#include <EEPROMex.h>
#include <Adafruit_NeoPixel.h>
#include <CmdMessenger.h>
#include "Andi_Bibilothek_BalancingBot.h"
#include <PID_v1_Andi.h>

unsigned long Zykluszeit=0;				//akutelle Zykluszeit
bool MotorenEINAUS = false;				//Motoren Ein Aus Schalter 


double Sollwert_PID_Winkel, Eingang_PID_Winkel, Ausgang_PID_Winkel;//PID Regler Werte
double Sollwert_PID_Geschwindigkeit, Eingang_PID_Geschwindigkeit, Ausgang_PID_Geschwindigkeit;//PID Regler Werte für Geschwindigkeitsregler


PID PID_Regler_Winkel(&Eingang_PID_Winkel, &Ausgang_PID_Winkel, &Sollwert_PID_Winkel, 10,0,0,DIRECT);//PID-Regler für Wickelsteuerung
PID PID_Regler_Geschwindigkeit(&Eingang_PID_Geschwindigkeit,&Ausgang_PID_Geschwindigkeit,&Sollwert_PID_Geschwindigkeit,1,1,1,DIRECT);

Stepper_Motor Motor_Links(false,Pin_Step_Links,Pin_FAULT_Links,Pin_DIR_Links,Pin_Sleep_Motortreiber,Pin_Reset_Motortreiber,Pin_Stepmode_MS1,Pin_Stepmode_MS2,Pin_ENABLE_Motortreiber);
Stepper_Motor Motor_Rechts(true,Pin_Step_Rechts,Pin_FAULT_Rechts,Pin_DIR_Rechts,Pin_Sleep_Motortreiber,Pin_Reset_Motortreiber,Pin_Stepmode_MS1,Pin_Stepmode_MS2,Pin_ENABLE_Motortreiber);

CmdMessenger cmdMessenger = CmdMessenger(Serial);

 //the setup function runs once when you press reset or power the board
void setup() 
{
	Pin_Setup();

	analogWrite(Pin_Platinenluefter,207);//Platinenlüfter auf 80% stellen

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
	Statusmeldung();
}


 //the loop function runs over and over again until power down or reset
void loop() 
{
	Zykluszeit = Zykluszeit_Messung();
	Zeit_Takt_20ms();
	
	//Akkuueberwachung(Pin_Akku1_Messung,Pin_Akku2_Messung);
	//MPU Zyklus nur ausführen wenn MPU nicht gestört ist
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
		Eingang_PID_Winkel=GET_KalmanWinkelX();
		//PID-Regler ausführen
		PID_Regler_Winkel.Compute();//PID-Regler für die Winkelsteuerung zyklisch ausführen
		Motoren_Steuerung(Ausgang_PID_Winkel,0,0);
	}

	Umkippschutz(20,GET_KalmanWinkelX());

	//PID_Regler_Geschwindigkeit.Compute();
	Akkuueberwachung(Pin_Akku1_Messung,Pin_Akku2_Messung);
	Lueftersteuerung_Temperatur(GET_MPU_Temperatur(),Pin_Gehaeuseluefter);//Gehäuselüftersteuerung

	cmdMessenger.feedinSerialData();//cmdMessenger Datenauslesen und Callbacks auslösen
}
