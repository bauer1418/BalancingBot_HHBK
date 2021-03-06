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
//
	//#include "Andi_ZeitTakt_Funktionen.h"

	//Motoren
	const int Steps_pro_Umdrehnung=200;			//Steps pro Umdrehung der Nema17 Motoren
	const int max_Drehzahl=200;					//maximale Drehzahl der Nema17 Motoren durch Tests ermittelt
	const volatile int DRV8825_Step_Pause=10;	//Angabe aus dem Datenblatt Seite 7 Timing Requirements 1,9�s bzw. 650ns  da Kleinstezeitverz�gerung micros ist auf 2�s gestellt

	class Stepper_Motor
	{
	public:
		Stepper_Motor(bool Invertiert, int PIN_Step,int PIN_Fehler, int PIN_Richtung,int PIN_Sleep,int PIN_Reset,int PIN_Stepmode_MS1,int PIN_Stepmode_MS2,int PIN_Enable);
		void Aktiv_Schalten(bool Schaltbefehl);//Schaltbefehl true = aktiv schalten
		int Status(void);
		enum Motortreiber_Status{Aktiv, Deaktiv, Fehler};
		void Fehler_quittieren(void);//Motortreiber zur�cksetzen
		void StepMode_setzen(int StepMode);
		enum StepModes{Vollschritt, Halbschritt, Viertelschritt, Achtelschritt};
		/*bool Step (double Drehzahl, double Prozent);*/
		volatile bool Step();
		enum Richtungen{
		Vorwaerts,							//Motorrichtung Vorwaerts
		Rueckwaerts							//Motorrichtung R�ckw�rts
		};
		volatile void Drehzahl_festlegen(double Drehzahl, double Prozent);
		volatile void Richtung_einstellen(double Drehzahl);
		double Solldrehzahl;//Aktuelle Solldrehzahl
	private:
		int Private_PIN_Step, Private_PIN_Fehler, Private_PIN_Richtung, Private_PIN_Sleep, Private_PIN_Reset, Private_PIN_Stepmode_MS1, Private_PIN_Stepmode_MS2, Private_PIN_Enable;
		bool Private_Invertiert;//wenn false keine �nderng wenn true Wirkung umkehren
		bool Private_Motor_aktiv;//Motor aktiv (true)oder deaktiv(false) 
		double Private_Drehzahl;//Aktuelle Solldrehzahl
		volatile double Private_Pausenzeit;//Pausenzeit bis zum n�chsten Step bei aktueller Drehzahl
		double Versatz_Rechner(double Drehzahl, double Prozent_Versatz);
		double Pausenzeit_Rechner(double Drehzahl);
		volatile unsigned long Private_letzer_Step_Zeitpunkt;//Zeitpunkt des Letzten Steps
		bool Schalt_Zeitpunkt (unsigned long Pausenzeit, unsigned long letzter_Schaltzeitpunkt);
		/*void Richtung_einstellen(int Richtung);*/

		int Private_aktueller_Stepmodus = 0;//Eingestellter Stepmodus des Motors
	};

	Stepper_Motor::Stepper_Motor(bool Invertiert, int PIN_Step,int PIN_Fehler, int PIN_Richtung,int PIN_Sleep,int PIN_Reset,int PIN_Stepmode_MS1,int PIN_Stepmode_MS2,int PIN_Enable)
	{
		//�bertragung der Parameter in Private Variablen
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

	volatile void Stepper_Motor::Drehzahl_festlegen(double Drehzahl, double Prozent)
	{
		Solldrehzahl = Drehzahl;
		if (Private_aktueller_Stepmodus== StepModes::Halbschritt)
		{
			Drehzahl = Drehzahl * 2.0;
		}
		else if (Private_aktueller_Stepmodus == StepModes::Viertelschritt)
		{
			Drehzahl = Drehzahl * 4.0;
		}
		else if (Private_aktueller_Stepmodus == StepModes::Achtelschritt)
		{
			Drehzahl = Drehzahl * 8.0;
		}

		//if (Private_Invertiert == true)
		//{
		//	if (Drehzahl > 0.0)
		//	{
		//		digitalWrite(Private_PIN_Richtung, true);
		//	}
		//	else //(Richtung==Rueckwaerts)
		//	{
		//		digitalWrite(Private_PIN_Richtung, false);
		//	}
		//}
		//else //(Private_Invertiert==false)
		//{
		//	if (Drehzahl >0.0)
		//	{
		//		digitalWrite(Private_PIN_Richtung, false);
		//	}
		//	else //(Richtung==Rueckwaerts)
		//	{
		//		digitalWrite(Private_PIN_Richtung, true);
		//	}
		//}

		/*if (Drehzahl>0.0)
			{
				Richtung_einstellen(Vorwaerts);
			}
		else
			{
				Richtung_einstellen(Rueckwaerts);
			}*/
		
		Private_Drehzahl = Stepper_Motor::Versatz_Rechner(Drehzahl, Prozent);
		Private_Pausenzeit = Stepper_Motor::Pausenzeit_Rechner(Private_Drehzahl);
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
			Private_aktueller_Stepmodus = Vollschritt;
			digitalWrite(Private_PIN_Stepmode_MS1,false);
			digitalWrite(Private_PIN_Stepmode_MS2,false);
		}
		else if (StepMode==Halbschritt)
		{
			Private_aktueller_Stepmodus = Halbschritt;
			digitalWrite(Private_PIN_Stepmode_MS1,true);
			digitalWrite(Private_PIN_Stepmode_MS2,false);
		}
		else if (StepMode==Viertelschritt)
		{
			Private_aktueller_Stepmodus = Viertelschritt;
			digitalWrite(Private_PIN_Stepmode_MS1,false);
			digitalWrite(Private_PIN_Stepmode_MS2,true);
		}
		else if (StepMode==Achtelschritt)
		{
			Private_aktueller_Stepmodus = Achtelschritt;
			digitalWrite(Private_PIN_Stepmode_MS1,true);
			digitalWrite(Private_PIN_Stepmode_MS2,true);
		}
	}
	volatile bool Stepper_Motor::Step ()
	{
		if (Private_Motor_aktiv==true)
		{
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
	//Versatzrechner um aus einer Drehzahl die neue Solldrehzahl zu bekommen um z.B.: ein Rad 10% schneller laufen zu Lassen f�r Kurvenfahrten
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
		if (Drehzahl!=0.0)
		{
			double Steps_pro_Sekunde = Drehzahl*Steps_pro_Umdrehnung/60;
			double Pausenzeit = 1/Steps_pro_Sekunde*1000*1000;
			if (Pausenzeit<0)
			{
			Pausenzeit=Pausenzeit*-1.0;
			}
			return Pausenzeit;
		}
		return 30003000.3;//Wert f�r 0,01
	}
	//Zeit�berpr�fung ob bereits die angegebende Pausenzeit abgelaufen ist
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
	//Richtung f�r den Motor festlegen
	volatile void Stepper_Motor::Richtung_einstellen(double Drehzahl)
	{
		if (Private_Invertiert==true)
		{
			if (Drehzahl>0.0)
			{
				digitalWrite(Private_PIN_Richtung,true);
			}
			else //(Richtung==Rueckwaerts)
			{
				digitalWrite(Private_PIN_Richtung,false);
			}
		}
		else //(Private_Invertiert==false)
		{
			if (Drehzahl>0.0)
			{
				digitalWrite(Private_PIN_Richtung,false);
			}
			else //(Richtung==Rueckwaerts)
			{
				digitalWrite(Private_PIN_Richtung,true);
			}
		}

	}
	//void Stepper_Motor::Richtung_einstellen(int Richtung)
	//{
		//if (Private_Invertiert==true)
		//{
		//	if (Richtung==Vorwaerts)
		//	{
		//		digitalWrite(Private_PIN_Richtung,true);
		//	}
		//	else //(Richtung==Rueckwaerts)
		//	{
		//		digitalWrite(Private_PIN_Richtung,false);
		//	}
		//}
		//else //(Private_Invertiert==false)
		//{
		//	if (Richtung==Vorwaerts)
		//	{
		//		digitalWrite(Private_PIN_Richtung,false);
		//	}
		//	else //(Richtung==Rueckwaerts)
		//	{
		//		digitalWrite(Private_PIN_Richtung,true);
		//	}
		//}



	//}

#endif //Ende der Define Anweisung