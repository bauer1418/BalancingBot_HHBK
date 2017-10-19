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
#include "Messenger_Enum.h"



//Variablen
//Fehler und Status�berwachung
byte Status = 0;						//Systemstatus Variable Bedeutung siehe Enum Statusmeldungen
byte Fehlerspeicher=0;					//Fehlerstatus Bedeutung siehe Enum Fehlermeldungen

unsigned long letzer_Step_Links=0;		//Zeitpunkt des letzten Steps f�r Motor Links
unsigned long letzer_Step_Rechts=0;		//Zeitpunkt des letzten Steps f�r Motor Rechts




//L�fter
int LuefterPlatineDrehzahl=0;			//L�fterdrehzahl in % f�r Platine
int LuefterGehaeuseDrehzahl=0;			//L�fterdrehzahl in % f�r Geh�use

//Akkumessungen
double Akkuspannung1 = 0.00;			//Akkuspannung 1 aus der Akku�berwachung
double Akkuspannung2 = 0.00;			//Akkuspannung 2 aus der Akku�berwachung
double AkkuSpannungMin = 6.00;			//AkkuEntladespannung 0%
double AkkuSpannungKritisch = 6.24;		//Akkuspannung 10%
double AkkuSpannungNiedrig = 6.60;		//Akkuspannung 25%
double AkkuSpannungMax = 8.40;			//Akkuspannung bei Vollgeladenem Akku 100%
double Akku_Prozent = 0;					//Akkustand in Prozent gemittelt �ber beide Packs
double AkkuSpannungGesamt = 0.0;		//Akkuspannung beide Packs
double Akku_Messbereich = 0.00;			//Messbereich f�r die Akkuspannung
const int SpannungsmessungR1 = 22000;	//Vorwiderstand f�r Akkumessungen in Ohm
const int SpannungsmessungR2 = 3900;	//Messwiderstand f�r Akkumessungen in Ohm

//Pinnummern �bersicht
const int Pin_Stepmode_MS1  = 2	;		//DigitalOutput Stepmode Umschaltung 1
const int Pin_NeopixelData = 3;			//DigitalOutput Neopixel Daten Ausgang
const int Pin_Stepmode_MS2 = 4;			//DigitalOutput Stepmode Umschaltung 1
const int Pin_Platinenluefter = 5;		//PWMOutput Platinenl�fter Drehzahl f�r Transistor Q2
const int Pin_Gehaeuseluefter = 6;		//PWMOutput Platinenl�fter Drehzahl f�r Transistor Q3
const int Pin_DIR_Rechts = 7;			//DigitalOutput Richtungsauswahl Motor Rechts
const int Pin_DIR_Links = 8;			//DigitalOutput Richtungsauswahl Motor Links
const int Pin_Step_Rechts = 9;			//PWMOutput Step Befehl f�r Motor Rechts 
const int Pin_Step_Links = 10;			//PWMOutput Step Befehl f�r Motor Links
const int Pin_Sleep_Motortreiber = 11;	//DigitalOutput Motortreiber in den Schlafmodus versetzen 0=Schlafmodus 1=Betrieb
const int Pin_Reset_Motortreiber = 12;	//DigitalOutput Fehler des Mototreibers zur�cksetzen 1=Aktiv
const int Pin_ENABLE_Motortreiber = 13;	//DigitalOutput Motortreiber Aktivieren 0=Aktiv 1=Inaktiv
const int Pin_FAULT_Links = 14;			//DigitalInput Fehlereingang Motortreiber Links
const int Pin_FAULT_Rechts =15;			//DigitalInput Fehlereingang Motortreiber Rechts
const int Pin_Reserve_A2 = 16;			//Analog Reserve Pin
const int Pin_Reserve_A3 = 17;			//Analog Reserve Pin
const int Pin_Akku1_Messung = 18;		//AnalogInput Spannungsmessung Akku 1
const int Pin_Akku2_Messung = 19;		//AnalogInput Spannungsmessung Akku 2



//Parameter f�r die Funktionen in dieser Library

unsigned long  Startzeitpunkt_Zeit_Takt_20ms=0;//Zeitpunkt der letzen Ausf�hrung des 20ms Takts
unsigned long  Startzeitpunkt_Zykluszeit_Messung=0;//Zwischenspeicher f�r Zykluszeitmessung in �s
byte Anzahl_20ms_Takte=0;//Zwischenspeicher f�r 100ms und 1s Zeit Takte

Stepper_Motor Motor_Links(true,Pin_Step_Links,Pin_FAULT_Links,Pin_DIR_Links,Pin_Sleep_Motortreiber,Pin_Reset_Motortreiber,Pin_Stepmode_MS1,Pin_Stepmode_MS2,Pin_ENABLE_Motortreiber);
Stepper_Motor Motor_Rechts(false,Pin_Step_Rechts,Pin_FAULT_Rechts,Pin_DIR_Rechts,Pin_Sleep_Motortreiber,Pin_Reset_Motortreiber,Pin_Stepmode_MS1,Pin_Stepmode_MS2,Pin_ENABLE_Motortreiber);


//Pin Setup Routine um alle Pins in den Richtigen Pin Mode zuversetzen und Passendere Namen zugeben 
void Pin_Setup()
{
	//PinMode f�r Pins setzen (au�er f�r Motoren siehe Stepper Class)
	pinMode(Pin_NeopixelData,OUTPUT);
	pinMode(Pin_Platinenluefter,OUTPUT);
	pinMode(Pin_Gehaeuseluefter,OUTPUT);
	pinMode(Pin_Akku1_Messung,INPUT);
	pinMode(Pin_Akku2_Messung,INPUT);
}
//Zeit�berpr�fung ob bereits die angegebende Pausenzeit abgelaufen ist
//Zeiten in micros
bool Schalt_Zeitpunkt (double Pausenzeit, double letzter_Schaltzeitpunkt)
{
	if (letzter_Schaltzeitpunkt+Pausenzeit<micros())
	{
		return true;//Zeit erreicht
	}
	else
	{
		return false;//Zeit noch nicht erreicht
	}
}


void test()
{

}




//L�ftersteuerung �ber eine Temperatur mit Schreiben des analogen PWM Ausgangs
void Lueftersteuerung_Temperatur(double Temperatur, int Luefter_Pin)
{
	if (Temperatur>=40.0)
	{
		analogWrite(Luefter_Pin,255);//100%
	}
	else if (Temperatur>=35.0)
	{
		analogWrite(Luefter_Pin,204);//80%
	}
		else if (Temperatur>=30.0)
	{
		analogWrite(Luefter_Pin,153);//60%
	}
		else
	{
		analogWrite(Luefter_Pin,128);//50%
	}
}

//Die Steuerbefehle f�r die Stepper direkt in das Ausgangsregister PORTB schreiben um die Motoren zeitgleich zu steuern.
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
		PORTB=PORTB&B10111111;
	}
	else if (MotorLinks_Step==true && MotorRechts_Step==false)//Links einen Schritt
	{
		/*PORTB=PORTB|B00100000;
		delayMicroseconds(DRV8825_Step_Pause);
		PORTB=PORTB&B11011111;*/
		digitalWrite(Pin_Step_Links,true);
		delayMicroseconds(2500);
		digitalWrite(Pin_Step_Links,false);

	}
	else if (MotorLinks_Step==false && MotorRechts_Step==false)//beide keinen Schritt
	{
		PORTB=PORTB&B10011111;
	}

}


//Unterprogramm zur Motorensteuerung
//Drehzahl ist ben�tigte Drehzahl f�r Balancing
//Prozent_Rechts/Links entspricht dem Sollwert f�r den jewaligen Motor in Prozent 100% => Sollwert==Motorwert
void Motoren_Steuerung(double Drehzahl, double Prozent_Rechts, double Prozent_Links)
{
	Ausgangsregister_schreiben(Motor_Links.Step(Drehzahl,Prozent_Links),Motor_Rechts.Step(Drehzahl,Prozent_Rechts));
	//double Drehzahl_Rechts= Versatz_Rechner(Drehzahl,Versatz_Rechts);
	//double Drehzahl_Links= Versatz_Rechner(Drehzahl,Versatz_Links);
	//double Pausenzeit_Links=Pausenzeit_Rechner(Drehzahl_Links);
	//double Pausenzeit_Rechts=Pausenzeit_Rechner(Drehzahl_Rechts);
	//Ausgangsregister_schreiben(Schalt_Zeitpunkt(Pausenzeit_Links,letzer_Step_Links),Schalt_Zeitpunkt(Pausenzeit_Rechts,letzer_Step_Rechts));

}

//Fehlerauswertung
void Fehlerauswertung()
{

}

//Spannungsteilerberechnung f�r Spannungskontrolle
double Spannungsteiler (double R1, double R2, int AnalogEingangsPin)
	
{
	//double Strom=0.00;
	//double Spannung=-70.00;
	//pinMode(AnalogEingangsPin, INPUT);
	//Strom=(analogRead(AnalogEingangsPin)*5.00/1024.00)/R1;
	//Spannung=(R1+R2)*Strom;
	//return Spannung;
	double Teiler=R2/(R1+R2);
	pinMode(AnalogEingangsPin, INPUT);
	double Eingang=analogRead(AnalogEingangsPin);
	double Spannung=(Eingang*5.0)/1024;
	Spannung=Spannung/Teiler;
	return Spannung;

}

//Spannungsbereich f�r die Akkumessungen berechnen Beispiel 8,4V-6V=2,4V
double Akku_Messbereich_Berechnen(double AkkuMin, double AkkuMax)
{
	double Messbereich =0.00;
	Messbereich= AkkuMax-AkkuMin;
	return Messbereich;
}

//Akkumessungen auswerten und in den Variablen die entsprechende Werte eintragen
//Return TRUE Akkustatus in Ordnung FALSE Akkustatus niedrig oder kristisch
bool Akkuueberwachung (int AkkuPin1, int AkkuPin2)
{
	if (Akku_Messbereich==0.00)
	{
		Akku_Messbereich=Akku_Messbereich_Berechnen(AkkuSpannungMin,AkkuSpannungMax);
	}

	//Akkumessung f�r akku 2 Akku GND auf A6
	//Akkumessung f�r akku 1 Akkugesammnt GND auf A7
	Akkuspannung2 = Spannungsteiler(SpannungsmessungR1,SpannungsmessungR2,AkkuPin1);
	AkkuSpannungGesamt = Spannungsteiler(SpannungsmessungR1,SpannungsmessungR2,AkkuPin2);
	Akkuspannung1 = AkkuSpannungGesamt-Akkuspannung2;
	Akku_Prozent = (((Akkuspannung1-AkkuSpannungMin)/Akku_Messbereich*100)+((Akkuspannung2-AkkuSpannungMin)/Akku_Messbereich*100)/2);



	if ((Akkuspannung1 <=  AkkuSpannungKritisch || Akkuspannung2 <= AkkuSpannungKritisch))
	{
		Status= Akku_kritisch;
		return false;

	}
   else if (Akkuspannung1 <  AkkuSpannungNiedrig || Akkuspannung2 < AkkuSpannungNiedrig)
	{
		Status=Akkustand_niedrig;
		return false;
	}
	else
	{
		if (Status==AkkuSpannungNiedrig||Status==AkkuSpannungKritisch)
		{
			Status=System_Bereit;
		}
		return true;
	}
}



//Zeitmessung f�r 20ms Takt
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


//Muss im loop() ausgef�hrt werden
//Return Zykluszeit in �s
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