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

#include <PID_v1_Andi.h>
#include "Messenger_Enum.h"


byte Status = 0;						//Systemstatus Variable Bedeutung siehe Enum Statusmeldungen
byte Fehlerspeicher=0;
	
double Akkuspannung1 = 0.00;			//Akkuspannung 1 aus der Akkuüberwachung
double Akkuspannung2 = 0.00;			//Akkuspannung 2 aus der Akkuüberwachung
double AkkuSpannungMin = 6.00;			//AkkuEntladespannung 0%
double AkkuSpannungKritisch = 6.24;		//Akkuspannung 10%
double AkkuSpannungNiedrig = 6.60;		//Akkuspannung 25%
double AkkuSpannungMax = 8.40;			//Akkuspannung bei Vollgeladenem Akku 100%
byte Akku_Prozent = 0;					//Akkustand in Prozent gemittelt über beide Packs
double AkkuSpannungGesamt = 0.0;		//Akkuspannung beide Packs
double Akku_Messbereich = 0.00;			//Messbereich für die Akkuspannung
const int SpannungsmessungR1 = 22000;	//Vorwiderstand für Akkumessungen in Ohm
const int SpannungsmessungR2 = 3900;	//Messwiderstand für Akkumessungen in Ohm
unsigned long Zykluszeit=0;				//akutelle Zykluszeit
bool MotorenEINAUS = false;				//Motoren Ein Aus Schalter 


double Sollwert_PID_Winkel, Eingang_PID_Winkel, Ausgang_PID_Winkel;//PID Regler Werte
double Sollwert_PID_Geschwindigkeit, Eingang_PID_Geschwindigkeit, Ausgang_PID_Geschwindigkeit;//PID Regler Werte für Geschwindigkeitsregler

PID PID_Regler_Winkel(&Eingang_PID_Winkel, &Ausgang_PID_Winkel, &Sollwert_PID_Winkel, 10,0,0,DIRECT);//PID-Regler für Wickelsteuerung
PID PID_Regler_Geschwindigkeit(&Eingang_PID_Geschwindigkeit,&Ausgang_PID_Geschwindigkeit,&Sollwert_PID_Geschwindigkeit,1,1,1,DIRECT);


CmdMessenger cmdMessenger = CmdMessenger(Serial);

 //the setup function runs once when you press reset or power the board
void setup() 
{
	NeoPixel_Setup(25);

	Setup_cmdMessenger();
	
	Serial.begin(115200);
	
	Pin_Setup();
	
	if (MPUsetup==false)
	{
		Status=Fehler;
		Fehlerspeicher=8;
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
	/*if (Fehlerspeicher!=MPU_NOT_FOUND || Fehlerspeicher!=MPU_READ_FAILED || Fehlerspeicher!=MPU_READ_TIMEOUT || Fehlerspeicher!=MPU_Write_FAILED)
	{*/

		MPU_Zyklus();
		Zyklusdaten_senden();
	/*}*/
	
	if (MotorenEINAUS==true)
	{
		//Winkel auslesen
		Eingang_PID_Winkel=GET_KalmanWinkelX();
		//PID-Regler ausführen
		PID_Regler_Winkel.Compute();
	}

	Umkippschutz(20,Eingang_PID_Winkel);
	//Umkippschutz auswerten
	//Motoren einstellen
	//PID_Regler_Geschwindigkeit.Compute();
	

	cmdMessenger.feedinSerialData();//cmdMessenger Datenauslesen und Callbacks auslösen
}



//Akkumessungen auswerten und in den Variablen die entsprechende Werte eintragen
//Return TRUE Akkustatus in Ordnung FALSE Akkustatus niedrig oder kristisch
bool Akkuueberwachung (int AkkuPin1, int AkkuPin2)
{
	if (Akku_Messbereich==0.00)
	{
		Akku_Messbereich_Berechnen(AkkuSpannungMin,AkkuSpannungMax);
	}

	//Akkumessung für akku 2 Akku GND auf A6
	//Akkumessung für akku 1 Akkugesammnt GND auf A7
	Akkuspannung2 = Spannungsteiler(SpannungsmessungR1,SpannungsmessungR2,AkkuPin2);
	AkkuSpannungGesamt = Spannungsteiler(SpannungsmessungR1,SpannungsmessungR2,AkkuPin1);
	Akkuspannung1 = AkkuSpannungGesamt-Akkuspannung2;
	Akku_Prozent = (((Akkuspannung1-AkkuSpannungMin)/Akku_Messbereich*100)+((Akkuspannung2-AkkuSpannungMin)/Akku_Messbereich*100)/2);


	if ((Akkuspannung1 <=  AkkuSpannungKritisch || Akkuspannung2 <= AkkuSpannungKritisch))
	{
		Status= Akku_kritisch;
		return false;

	}
   else if (Akkuspannung1 <  AkkuSpannungNiedrig || Akkuspannung2 < AkkuSpannungNiedrig)
	{
		return false;
	}
	else
	{
		return true;
	}
}

