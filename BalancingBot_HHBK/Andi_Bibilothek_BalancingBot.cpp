/*
 Name:		BalancingBot_HHBK.ino
 Created:	23.07.2017 14:32:37
 Author:	Andreas Bauer
*/


#include "Andi_Bibilothek_BalancingBot.h"
#include "Messenger_Enum.h"


//Parameter für die Funktionen in dieser Library
unsigned long Startzeitpunkt_Zeit_Takt_20ms=0;
unsigned long Startzeitpunkt_Zykluszeit_Messung=0;

//Pin Setup Routine um alle Pins in den Richtigen Pin Mode zuversetzen und Passendere Namen zugeben 
void Pin_Setup()
{
	
	//PinMode für alle Pins setzen
	pinMode(Pin_Stepmode_MS1,OUTPUT);
	pinMode(Pin_NeopixelData,OUTPUT);
	pinMode(Pin_Stepmode_MS2,OUTPUT);
	pinMode(Pin_Platinenluefter,OUTPUT);
	pinMode(Pin_Gehaeuseluefter,OUTPUT);
	pinMode(Pin_DIR_Rechts,OUTPUT);
	pinMode(Pin_DIR_Links,OUTPUT);
	pinMode(Pin_Step_Rechts,OUTPUT);
	pinMode(Pin_Step_Links,OUTPUT);
	pinMode(Pin_Sleep_Motortreiber,OUTPUT);
	pinMode(Pin_Reset_Motortreiber,OUTPUT);
	pinMode(Pin_ENABLE_Motortreiber,OUTPUT);
	pinMode(Pin_FAULT_Links,INPUT);
	pinMode(Pin_FAULT_Rechts,INPUT);
	pinMode(Pin_Akku1_Messung,INPUT);
	pinMode(Pin_Akku2_Messung,INPUT);

}

//Spannungsteilerberechnung für Spannungskontrolle
double Spannungsteiler (double R1, double R2, int AnalogEingangsPin)
	
{
	double Strom=0.00;
	double Spannung=-70.00;
	pinMode(AnalogEingangsPin, INPUT);
	Strom=(analogRead(AnalogEingangsPin)*5.00/1024.00)/R1;
	Spannung=(R1+R2)*Strom;
	return Spannung;
}



//Spannungsbereich für die Akkumessungen berechnen Beispiel 8,4V-6V=2,4V
double Akku_Messbereich_Berechnen(double AkkuMin, double AkkuMax)
{
	double Messbereich =0.00;
	Messbereich= AkkuMax-AkkuMin;
	return Messbereich;
}

//Zeitmessung für 20ms Takt
//Return True 20ms seit dem letzten Takt abgelaufen
bool Zeit_Takt_20ms()
{
	Startzeitpunkt_Zeit_Takt_20ms=micros();
	if (micros()>=Startzeitpunkt_Zeit_Takt_20ms+20000)
	{
		return true;
	}
	else
	{
		return false;
	}

}
//Zykluszeit messen
//Muss im loop() ausgeführt werden
//Return Zykluszeit in µs
unsigned long Zykluszeit_Messung()
{
	return micros()-Startzeitpunkt_Zykluszeit_Messung;
}
