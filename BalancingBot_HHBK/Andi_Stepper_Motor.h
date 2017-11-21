/*
 Name:		Andi_Stepper_Motor.h
 Created:	15.10.2017 15:38:15
 Author:	Andreas Bauer
*/
#ifndef _ANDI_STEPPER_MOTOR_h
	#define _ANDI_STEPPER_MOTOR_h

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "arduino.h"
	#else
		#include "WProgram.h"
	#endif

	//#include "Andi_ZeitTakt_Funktionen.h"

	//Motoren
	const int Steps_pro_Umdrehnung=200;		//Steps pro Umdrehung der Nema17 Motoren
	const int max_Drehzahl=100;				//maximale Drehzahl der Nema17 Motoren durch Tests ermittelt
	const int DRV8825_Step_Pause=2;			//Angabe aus dem Datenblatt Seite 7 Timing Requirements 1,9µs bzw. 650ns  da Kleinstezeitverzögerung micros ist auf 2µs gestellt

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
		bool Step (double Drehzahl, double Prozent);
		enum Richtungen{
		Vorwaerts,							//Motorrichtung Vorwaerts
		Rueckwaerts							//Motorrichtung Rückwärts
		};
	private:
		int Private_PIN_Step, Private_PIN_Fehler, Private_PIN_Richtung, Private_PIN_Sleep, Private_PIN_Reset, Private_PIN_Stepmode_MS1, Private_PIN_Stepmode_MS2, Private_PIN_Enable;
		bool Private_Invertiert;//wenn false keine Änderng wenn true Wirkung umkehren
		bool Private_Motor_aktiv;//Motor aktiv (true)oder deaktiv(false) 
		double Private_Drehzahl;//Aktuelle Solldrehzahl
		double Private_Pausenzeit;//Pausenzeit bis zum nächsten Step bei aktueller Drehzahl
		double Versatz_Rechner(double Drehzahl, double Prozent_Versatz);
		double Pausenzeit_Rechner(double Drehzahl);
		unsigned long Private_letzer_Step_Zeitpunkt;//Zeitpunkt des Letzten Steps
		bool Schalt_Zeitpunkt (unsigned long Pausenzeit, unsigned long letzter_Schaltzeitpunkt);
		void Richtung_einstellen(int Richtung);
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
		digitalWrite(Private_PIN_Enable,true);
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
	bool Stepper_Motor::Step (double Drehzahl, double Prozent)
	{
		if (Private_Motor_aktiv==true)
		{
			if (Drehzahl>0)
			{
				Richtung_einstellen(Vorwaerts);
			}
			else
			{
				Richtung_einstellen(Rueckwaerts);
			}
			Private_Drehzahl=Stepper_Motor::Versatz_Rechner(Drehzahl,Prozent);
			Private_Pausenzeit=Stepper_Motor::Pausenzeit_Rechner(Private_Drehzahl);
			if (Stepper_Motor::Schalt_Zeitpunkt(Private_Pausenzeit,Private_letzer_Step_Zeitpunkt)==true)
			{
				Private_letzer_Step_Zeitpunkt=micros();
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	//Versatzrechner um aus einer Drehzahl die neue Solldrehzahl zu bekommen um z.B.: ein Rad 10% schneller laufen zu Lassen für Kurvenfahrten
	//Prozent_Versatz in +/- % 
	//100% gibt Orgnialwert wieder
	//Return Drehzahl in U/min
	double Stepper_Motor::Versatz_Rechner(double Drehzahl, double Prozent_Versatz)
	{
		return Drehzahl*Prozent_Versatz/100;
	}
	//Pausenzeit berechnen aus der Drehzahl in U/min
	//Return Pausenzeit in micros
	double Stepper_Motor::Pausenzeit_Rechner(double Drehzahl)
	{
		double Steps_pro_Sekunde = Drehzahl*Steps_pro_Umdrehnung/60;
		double Pausenzeit = 1/Steps_pro_Sekunde*1000*1000;
		if (Pausenzeit<0)
		{
			Pausenzeit=Pausenzeit*-1;
		}
		return Pausenzeit;
	}
	//Zeitüberprüfung ob bereits die angegebende Pausenzeit abgelaufen ist
	//Zeiten in micros
	bool Stepper_Motor::Schalt_Zeitpunkt (unsigned long Pausenzeit, unsigned long letzter_Schaltzeitpunkt)
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
	//Richtung für den Motor festlegen
	void Stepper_Motor::Richtung_einstellen(int Richtung)
	{
		if (Private_Invertiert==true)
		{
			if (Richtung==Vorwaerts)
			{
				digitalWrite(Private_PIN_Richtung,true);
			}
			else if(Richtung==Rueckwaerts)
			{
				digitalWrite(Private_PIN_Richtung,false);
			}
		}
		else if (Private_Invertiert==false)
		{
			if (Richtung==Vorwaerts)
			{
				digitalWrite(Private_PIN_Richtung,false);
			}
			else if(Richtung==Rueckwaerts)
			{
				digitalWrite(Private_PIN_Richtung,true);
			}
		}



	}


#endif //Ende der Define Anweisung