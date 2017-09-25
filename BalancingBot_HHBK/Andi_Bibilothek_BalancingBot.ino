/*
 Name:		BalancingBot_HHBK.ino
 Created:	23.07.2017 14:32:37
 Author:	Andreas Bauer
*/

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "arduino.h"
	#else
		#include "WProgram.h"
	#endif



//Variablen
const int Steps_pro_Umdrehnung=200;		//Steps pro Umdrehung der Nema17 Motoren
const int max_Drehzahl=100;				//maximale Drehzahl der Nema17 Motoren durch Tests ermittelt
const int DRV8825_Step_Pause=2;			//Angabe aus dem Datenblatt Seite 7 Timing Requirements 1,9µs bzw. 650ns  da Kleinstezeitverzögerung micros ist auf 2µs gestellt
unsigned long letzer_Step_Links=0;		//Zeitpunkt des letzten Steps für Motor Links
unsigned long letzer_Step_Rechts=0;		//Zeitpunkt des letzten Steps für Motor Rechts

//Pinnummern Übersicht
const int Pin_Stepmode_MS1  = 2	;		//DigitalOutput Stepmode Umschaltung 1
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



//Parameter für die Funktionen in dieser Library

unsigned long  Startzeitpunkt_Zeit_Takt_20ms=0;//Zeitpunkt der letzen Ausführung des 20ms Takts
unsigned long  Startzeitpunkt_Zykluszeit_Messung=0;//Zwischenspeicher für Zykluszeitmessung in µs
byte Anzahl_20ms_Takte=0;//Zwischenspeicher für 100ms und 1s Zeit Takte

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

//Pausenzeit berechnen aus der Drehzahl in U/min
//Return Pausenzeit in micros
double Pausenzeit_Rechner(double Drehzahl)
{
	double Steps_pro_Sekunde = Drehzahl*Steps_pro_Umdrehnung/60;
	double Pausenzeit = 1/Steps_pro_Sekunde*1000;
	return Pausenzeit;
}

//Versatzrechner um aus einer Drehzahl die neue Solldrehzahl zu bekommen um z.B.: ein Rad 10% schneller laufen zu Lassen für Kurvenfahrten
//Prozent_Versatz in +/- % 
//0% gibt Orgnialwert wieder
//Return Drehzahl in U/min
double Versatz_Rechner(double Drehzahl, double Prozent_Versatz)
{
	return Drehzahl*Prozent_Versatz/100;
}
//Die Steuerbefehle für die Stepper direkt in das Ausgangsregister PORTB schreiben um die Motoren zeitgleich zu steuern.
void Ausgangsregister_schreiben(bool MotorLinks_Step, bool MotorRechts_Step)
{
	if (MotorLinks_Step==true && MotorRechts_Step==true)//beide einen Schritt
	{
		PORTB=PORTB|B01100000;
		delayMicroseconds(DRV8825_Step_Pause);
		PORTB=PORTB&B10011111;
	}
	else if (MotorLinks_Step==false && MotorRechts_Step==true)//Rechts einen Schritt
	{
		PORTB=PORTB|B01000000;
		delayMicroseconds(DRV8825_Step_Pause);
		PORTB=PORTB&B10011111;
	}
	else if (MotorLinks_Step==true && MotorRechts_Step==false)//Links einen Schritt
	{
		PORTB=PORTB|B00100000;
		delayMicroseconds(DRV8825_Step_Pause);
		PORTB=PORTB&B10011111;
	}
	else if (MotorLinks_Step==false && MotorRechts_Step==false)//beide keinen Schritt
	{
		PORTB=PORTB&B10011111;
	}

}

//Prüfung ob bereits die Zeit für den nächsten Schritt bereit ist
bool Step_Zeitpunkt (double Pausenzeit, double letzter_Schritt_Zeitpunkt)
{
	if (letzter_Schritt_Zeitpunkt+Pausenzeit<micros())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Motoren_Steuerung(double Drehzahl, double Versatz_Rechts, double Versatz_Links)
{
	double Drehzahl_Rechts= Versatz_Rechner(Drehzahl,Versatz_Rechts);
	double Drehzahl_Links= Versatz_Rechner(Drehzahl,Versatz_Links);
	double Pausenzeit_Links=Pausenzeit_Rechner(Drehzahl_Links);
	double Pausenzeit_Rechts=Pausenzeit_Rechner(Drehzahl_Rechts);
	Ausgangsregister_schreiben(Step_Zeitpunkt(Pausenzeit_Links,letzer_Step_Links),Step_Zeitpunkt(Pausenzeit_Rechts,letzer_Step_Rechts));
}

//Fehlerauswertung
void Fehlerauswertung()
{

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
//Return TRUE wenn 20ms seit dem letzten Takt abgelaufen sind
bool Zeit_Takt_20ms()
{
	if (Anzahl_20ms_Takte>50)
	{
		Anzahl_20ms_Takte=1;
	}
	if (micros()>=(Startzeitpunkt_Zeit_Takt_20ms+20000))
	{
		Anzahl_20ms_Takte++;
		Startzeitpunkt_Zeit_Takt_20ms=micros();
		return true;
	}
	else
	{
		return false;
	}
	
}
//Zeit Takt alle 100ms wird aus dem 20ms Takt generiert
//Return TRUE wenn 100ms seit dem letzten Takt abgelaufen sind
bool Zeit_Takt_100ms()
{
	if (Anzahl_20ms_Takte%5==0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//Zeit Takt alle 1s wird aus dem 20ms Takt generiert
//Return TRUE wenn 1s seit dem letzten Takt abgelaufen sind
bool Zeit_Takt_1s()
{
	if (Anzahl_20ms_Takte%50==0)
	{
		Anzahl_20ms_Takte=1;
		return true;
	}
	else
	{
		return false;
	}
}


//Muss im loop() ausgeführt werden
//Return Zykluszeit in µs
unsigned long Zykluszeit_Messung()
{
	unsigned long Ergebnis=0;
	Ergebnis= micros()-Startzeitpunkt_Zykluszeit_Messung;
	Startzeitpunkt_Zykluszeit_Messung=micros();
	return Ergebnis;
}

//Auswertung der Umkipperkennung
//RETURN TRUE wenn Umkipperkennung angesprochen hat
bool Umkippschutz(int MaxWinkel, double EingangsWinkel)
{
	if (EingangsWinkel>MaxWinkel || EingangsWinkel<-MaxWinkel)
	{
		return true;
	}
	else
	{
		return false;
	}
}