/*
 Name:		BalancingBot_HHBK.ino
 Created:	23.07.2017 14:32:37
 Author:	Andreas Bauer
*/

#include "Andi_Bibilothek_BalancingBot.h"
//Pin Setup Routine um alle Pins in den Richtigen Pin Mode zuversetzen und Passendere Namen zugeben 
void Pin_Setup()
{

	//Pinnummern Übersicht
	const int Pin_Stepmode_MS1 = 2;			//DigitalOutput Stepmode Umschaltung 1
	const int Pin_NeopixelData = 3;			//DigitalOutput Neopixel Daten Ausgang
	const int Pin_Stepmode_MS2 = 4;			//DigitalOutput Stepmode Umschaltung 1
	const int Pin_Platinenluefter = 5;		//PWMOutput Platinenlüfter Drehzahl für Transistor Q2
	const int Pin_Gehaeuseluefter = 6;		//PWMOutput Platinenlüfter Drehzahl für Transistor Q3
	const int Pin_DIR_Rechts = 7;			//DigitalOutput Richtungsauswahl Motor Rechts
	const int Pin_DIR_Links = 8;			//DigitalOutput Richtungsauswahl Motor Links
	const int Pin_Step_Rechts = 9;			//PWMOutput Step Befehl für Motor Rechts 
	const int Pin_Step_Links = 10;			//PWMOutput Step Befehl für Motor Links
	const int Pin_Sleep_Motortreiber = 11;	//DigitalOutput Motortreiber in den Schlafmodus versetzen 0=Schlafmodus 1=Betrieb
	const int Pin_Reset_Motortreiber = 12;	//DigitalOutput Fehler des Mototreibers zurücksetzen 1=Aktiv
	const int Pin_ENABLE_Motortreiber = 13;	//DigitalOutput Motortreiber Aktivieren 0=Aktiv 1=Inaktiv
	const int Pin_FAULT_Links = 14;			//DigitalInput Fehlereingang Motortreiber Links
	const int Pin_FAULT_Rechts =15;			//DigitalInput Fehlereingang Motortreiber Rechts
	const int Pin_Reserve_A2 = 16;			//Analog Reserve Pin
	const int Pin_Reserve_A3 = 17;			//Analog Reserve Pin
	const int Pin_Akku1_Messung = 18;		//AnalogInput Spannungsmessung Akku 1
	const int Pin_Akku2_Messung = 19;		//AnalogInput Spannungsmessung Akku 2


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

