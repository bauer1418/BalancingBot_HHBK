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
//Fehler und Statusüberwachung
byte Status = 0;						//Systemstatus Variable Bedeutung siehe Enum Statusmeldungen
byte Fehlerspeicher=0;					//Fehlerstatus Bedeutung siehe Enum Fehlermeldungen

//Motoren
const int Steps_pro_Umdrehnung=200;		//Steps pro Umdrehung der Nema17 Motoren
const int max_Drehzahl=100;				//maximale Drehzahl der Nema17 Motoren durch Tests ermittelt
const int DRV8825_Step_Pause=2;			//Angabe aus dem Datenblatt Seite 7 Timing Requirements 1,9µs bzw. 650ns  da Kleinstezeitverzögerung micros ist auf 2µs gestellt
unsigned long letzer_Step_Links=0;		//Zeitpunkt des letzten Steps für Motor Links
unsigned long letzer_Step_Rechts=0;		//Zeitpunkt des letzten Steps für Motor Rechts

enum Richtungen
{
	Vorwaerts,							//Motorrichtung Vorwaerts
	Rueckwaerts							//Motorrichtung Rückwärts
};


//Lüfter
int LuefterPlatineDrehzahl=0;			//Lüfterdrehzahl in % für Platine
int LuefterGehaeuseDrehzahl=0;			//Lüfterdrehzahl in % für Gehäuse

//Akkumessungen
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
	//PinMode für Pins setzen (außer für Motoren siehe Stepper Class)
	pinMode(Pin_NeopixelData,OUTPUT);
	pinMode(Pin_Platinenluefter,OUTPUT);
	pinMode(Pin_Gehaeuseluefter,OUTPUT);
	pinMode(Pin_Akku1_Messung,INPUT);
	pinMode(Pin_Akku2_Messung,INPUT);
}

class Stepper_Motor
{
public:
	Stepper_Motor(bool Invertiert, int PIN_Step,int PIN_Fehler, int PIN_Richtung,int PIN_Sleep,int PIN_Reset,int PIN_Stepmode_MS1,int PIN_Stepmode_MS2,int PIN_Enable);
	void Aktiv_Schalten(bool Schaltbefehl);//Schaltbefehl true = aktiv schalten
	int Status(void);
	enum Motortreiber_Status{Aktiv, Deaktiv, Fehler};
	void Fehler_quittieren(void);//Motortreiber zurücksetzen
	void StepMode_setzen(int StepMode);
	enum StepModes{Vollschritt, Halbschritt, Viertelschritt, Achtelschritt};
	bool Step (double Drehzahl, double Versatz);

private:
	int Private_PIN_Step, Private_PIN_Fehler, Private_PIN_Richtung, Private_PIN_Sleep, Private_PIN_Reset, Private_PIN_Stepmode_MS1, Private_PIN_Stepmode_MS2, Private_PIN_Enable;
	bool Private_Invertiert;//wenn false keine Änderng wenn true Wirkung umkehren
	bool Private_Motor_aktiv;//Motor aktiv (true)oder deaktiv(false) 
	double Private_Drehzahl;//Pausenzeit bis zum nächsten Step bei aktueller Drehzahl
	double Private_Pausenzeit;//Pausenzeit bis zum nächsten Step bei aktueller Drehzahl
};

Stepper_Motor::Stepper_Motor(bool Invertiert, int PIN_Step,int PIN_Fehler, int PIN_Richtung,int PIN_Sleep,int PIN_Reset,int PIN_Stepmode_MS1,int PIN_Stepmode_MS2,int PIN_Enable)
{
	//Übertragung der Parameter in Private Variablen
	Private_Invertiert=Invertiert;
	Private_PIN_Step=PIN_Step;
	Private_PIN_Fehler=PIN_Fehler;
	Private_PIN_Richtung=PIN_Richtung;
	Private_PIN_Sleep=PIN_Sleep; 
	Private_PIN_Reset=PIN_Reset;
	Private_PIN_Stepmode_MS1=PIN_Stepmode_MS1;
	Private_PIN_Stepmode_MS2=PIN_Stepmode_MS2;
	Private_PIN_Enable=PIN_Enable;

	//PIN-Setup
	pinMode(Private_PIN_Richtung,OUTPUT);
	pinMode(Private_PIN_Step,OUTPUT);
	pinMode(Private_PIN_Sleep,OUTPUT);
	pinMode(Private_PIN_Reset,OUTPUT);
	pinMode(Private_PIN_Enable,OUTPUT);
	pinMode(Private_PIN_Fehler,INPUT);
	pinMode(Private_PIN_Stepmode_MS1,OUTPUT);
	pinMode(Private_PIN_Stepmode_MS2,OUTPUT);

	//Motortreiber starten , muss aber nochmal mit Aktivschalten aktiviert werden!
	digitalWrite(Private_PIN_Reset,true);
	digitalWrite(Private_PIN_Sleep,true);

}
void Stepper_Motor::Aktiv_Schalten(bool Schaltbefehl)
{
	Private_Motor_aktiv=Schaltbefehl;
	if (Private_Motor_aktiv==true)
	{
		digitalWrite(Private_PIN_Enable,false);
	}
	else
	{
		digitalWrite(Private_PIN_Enable,true);
	}
}
int Stepper_Motor::Status(void)
{
	if (digitalRead(Private_PIN_Fehler)==true)
	{
		return Fehler;
	}
	else if (Private_Motor_aktiv==true)
	{
		return Aktiv;
	}
	else
	{
		return Deaktiv;
	}
}
void Stepper_Motor::Fehler_quittieren(void)
{
	digitalWrite(Private_PIN_Reset,false);
	delay(5);
	digitalWrite(Private_PIN_Reset,true);
}
void Stepper_Motor::StepMode_setzen(int StepMode)
{
	if (StepMode==Vollschritt)
	{
		digitalWrite(Private_PIN_Stepmode_MS1,false);
		digitalWrite(Private_PIN_Stepmode_MS2,false);
	}
	else if (StepMode==Halbschritt)
	{
		digitalWrite(Private_PIN_Stepmode_MS1,true);
		digitalWrite(Private_PIN_Stepmode_MS2,false);
	}
	else if (StepMode==Viertelschritt)
	{
		digitalWrite(Private_PIN_Stepmode_MS1,false);
		digitalWrite(Private_PIN_Stepmode_MS2,true);
	}
	else if (StepMode==Achtelschritt)
	{
		digitalWrite(Private_PIN_Stepmode_MS1,true);
		digitalWrite(Private_PIN_Stepmode_MS2,true);
	}
}
bool Stepper_Motor::Step (double Drehzahl, double Versatz)
{
	Private_Drehzahl=Versatz_Rechner(Drehzahl,Versatz);
	Private_Pausenzeit=Pausenzeit_Rechner(Private_Drehzahl);
}
void test()
{
	Stepper_Motor MotorTEST(false,1,2,3,4,5,6,7,8);
	
}




//Lüftersteuerung über eine Temperatur mit Schreiben des analogen PWM Ausgangs
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

void Motoren_Steuerung(double Drehzahl, double Versatz_Rechts, double Versatz_Links)
{
	//double Drehzahl_Rechts= Versatz_Rechner(Drehzahl,Versatz_Rechts);
	//double Drehzahl_Links= Versatz_Rechner(Drehzahl,Versatz_Links);
	//double Pausenzeit_Links=Pausenzeit_Rechner(Drehzahl_Links);
	//double Pausenzeit_Rechts=Pausenzeit_Rechner(Drehzahl_Rechts);
	//Ausgangsregister_schreiben(Schalt_Zeitpunkt(Pausenzeit_Links,letzer_Step_Links),Schalt_Zeitpunkt(Pausenzeit_Rechts,letzer_Step_Rechts));
	Ausgangsregister_schreiben();
}
//Zeitüberprüfung ob bereits die angegebende Pausenzeit abgelaufen ist
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