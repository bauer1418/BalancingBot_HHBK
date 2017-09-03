/*
 Name:		BalancingBot_HHBK.ino
 Created:	19.07.2017 02:36:37
 Author:	Andreas Bauer
*/

//Benötigte Bibilotheken
#include <Adafruit_NeoPixel.h>
#include <CmdMessenger.h>
#include "Andi_Bibilothek_BalancingBot.h"
#include <EEPROM.h>
#include <PID_v1_Andi.h>
#include "Messenger_Enum.h"

int Status = 0;							//Systemstatus Variable Bedeutung siehe Enum Statusmeldungen



	
double Akkuspannung1 = 0.00;			//Akkuspannung 1 aus der Akkuüberwachung
double Akkuspannung2 = 0.00;			//Akkuspannung 2 aus der Akkuüberwachung
double AkkuSpannungMin = 6.00;			//AkkuEntladespannung 0%
double AkkuSpannungKritisch = 6.24;		//Akkuspannung 10%
double AkkuSpannungNiedrig = 6.60;		//Akkuspannung 25%
double AkkuSpannungMax = 8.40;			//Akkuspannung bei Vollgeladenem Akku 100%
int Akku_Prozent = 0;					//Akkustand in Prozent gemittelt über beide Packs
double AkkuSpannungGesamt = 0.0;		//Akkuspannung beide Packs
double Akku_Messbereich = 0.00;
const int SpannungsmessungR1 = 22000;	//Vorwiderstand für Akkumessungen
const int SpannungsmessungR2 = 3900;	//Messwiderstand für Akkumessungen

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
	Akkuueberwachung(Pin_Akku1_Messung,Pin_Akku2_Messung);
	//Kalmanfilter ausführen
	//PID-Regler ausführen
	//Motoren einstellen
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