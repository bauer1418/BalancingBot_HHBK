/*
 Name:		BalancingBot_HHBK.ino
 Created:	23.07.2017 14:32:37
 Author:	Andreas Bauer
*/

#ifndef _ANDI_BIBILOTHEK_BALANCINGBOT_h
	#define _ANDI_BIBILOTHEK_BALANCINGBOT_h


		#if defined(ARDUINO) && ARDUINO >= 100
			#include "arduino.h"
		#else
			#include "WProgram.h"
		#endif
	#include "Messenger_Enum.h"
	#include <PID_v1_Andi.h>
	#include <CmdMessenger.h>
	#include "Andi_Stepper_Motor.h"
	#include "EEPROM.h"
	#include "MPU6050_BalancingBot_HHBK.h"

	//Variablen
	//Fehler und Statusüberwachung
	byte Status = 0;						//Systemstatus Variable Bedeutung siehe Enum Statusmeldungen
	byte Fehlerspeicher=0;					//Fehlerstatus Bedeutung siehe Enum Fehlermeldungen

	unsigned long letzer_Step_Links=0;		//Zeitpunkt des letzten Steps für Motor Links
	unsigned long letzer_Step_Rechts=0;		//Zeitpunkt des letzten Steps für Motor Rechts

	CmdMessenger cmdMessenger = CmdMessenger(Serial);


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
	double Akku_Prozent = 0;					//Akkustand in Prozent gemittelt über beide Packs
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

	Stepper_Motor Motor_Links(true,Pin_Step_Links,Pin_FAULT_Links,Pin_DIR_Links,Pin_Sleep_Motortreiber,Pin_Reset_Motortreiber,Pin_Stepmode_MS1,Pin_Stepmode_MS2,Pin_ENABLE_Motortreiber);
	Stepper_Motor Motor_Rechts(false,Pin_Step_Rechts,Pin_FAULT_Rechts,Pin_DIR_Rechts,Pin_Sleep_Motortreiber,Pin_Reset_Motortreiber,Pin_Stepmode_MS1,Pin_Stepmode_MS2,Pin_ENABLE_Motortreiber);

	
	double Sollwert_PID_Winkel, Eingang_PID_Winkel, Ausgang_PID_Winkel;//PID Regler Werte
	double Sollwert_PID_Geschwindigkeit, Eingang_PID_Geschwindigkeit, Ausgang_PID_Geschwindigkeit;//PID Regler Werte für Geschwindigkeitsregler


	PID PID_Regler_Winkel(&Eingang_PID_Winkel, &Ausgang_PID_Winkel, &Sollwert_PID_Winkel, 10,0,0,DIRECT);//PID-Regler für Wickelsteuerung
	PID PID_Regler_Geschwindigkeit(&Eingang_PID_Geschwindigkeit,&Ausgang_PID_Geschwindigkeit,&Sollwert_PID_Geschwindigkeit,1,1,1,DIRECT);


	//Im EEPROM gespeicherte Einstellungen
	struct Balancing_Bot_Einstellungen
	{
		int Werte_ueberschrieben;					//Anzahl der Überschreibvorgänge des EEPROMs
		double PID_Regler_Winkel_P;					//P-Einstellung für Winkel-Regler
		double PID_Regler_Winkel_I;					//I-Einstellung für Winkel-Regler
		double PID_Regler_Winkel_D;					//D-Einstellung für Winkel-Regler
		double PID_Regler_Geschwindigkeit_P;		//P-Einstellung für Geschwindigkeits-Regler
		double PID_Regler_Geschwindigkeit_I;		//I-Einstellung für Geschwindigkeits-Regler
		double PID_Regler_Geschwindigkeit_D;		//D-Einstellung für Geschwindigkeits-Regler
		int PID_Regler_Winkel_Min;					//Minimaler Ausgangswert des Winkel-Reglers
		int PID_Regler_Winkel_Max;					//Maximaler Ausgangswert des Winkel-Reglers
		int PID_Regler_Geschwindigkeit_Min;			//Minimaler Ausgangswert des Geschwindigkeits-Reglers
		int PID_Regler_Geschwindigkeit_Max;			//Maximaler Ausgangswert des Geschwindigkeits-Reglers
		byte Schrittmodus;							//Stepmodus der Motorregler
		byte Platinenluefter_Sollwert;				//Sollwert für Platinenlüfter PWM
		byte Gehaeuseluefter_Sollwert;				//Sollwert für Gehäuselüfter PWM
		byte NeoPixelRing_Helligkeit;				//Helligkeitseinstellung des NeoPixelRings
		double MPU_Offset_AcelX;					//Offsetwert für MPU AcelX
		double MPU_Offset_AcelY;					//Offsetwert für MPU AcelY
		double MPU_Offset_AcelZ;					//Offsetwert für MPU AcelZ
		double MPU_Offset_GyroX;					//Offsetwert für MPU GyroX
		double MPU_Offset_GyroY;					//Offsetwert für MPU GyroY
		double MPU_Offset_GyroZ;					//Offsetwert für MPU GyroZ
	};

	Balancing_Bot_Einstellungen System_Einstellungen;//Einstellungen die im EEPROM abgelegt werden können

	
//Daten aus dem EEPROM auslesen von Adresse 0 startend
//return Einstellungen
Balancing_Bot_Einstellungen Daten_aus_EEPROM_lesen()
{
	Balancing_Bot_Einstellungen EEPROM_Daten;
	EEPROM.get(0,EEPROM_Daten);
	return EEPROM_Daten;
}



void Einstellungen_mit_Standart_Werten_beschreiben()
{
	System_Einstellungen.Werte_ueberschrieben=0;					//Standartwert wie oft das EEPROM überschrieben wurde
	System_Einstellungen.PID_Regler_Winkel_P=0;						//P-Einstellung für Winkel-Regler
	System_Einstellungen.PID_Regler_Winkel_I=0;						//I-Einstellung für Winkel-Regler
	System_Einstellungen.PID_Regler_Winkel_D=0;						//D-Einstellung für Winkel-Regler
	System_Einstellungen.PID_Regler_Geschwindigkeit_P=0;			//P-Einstellung für Geschwindigkeits-Regler
	System_Einstellungen.PID_Regler_Geschwindigkeit_I=0;			//I-Einstellung für Geschwindigkeits-Regler
	System_Einstellungen.PID_Regler_Geschwindigkeit_D=0;			//D-Einstellung für Geschwindigkeits-Regler
	System_Einstellungen.PID_Regler_Winkel_Min=-120;				//Minimaler Ausgangswert des Winkel-Reglers
	System_Einstellungen.PID_Regler_Winkel_Max=120;					//Maximaler Ausgangswert des Winkel-Reglers
	System_Einstellungen.PID_Regler_Geschwindigkeit_Min=-5;			//Minimaler Ausgangswert des Geschwindigkeits-Reglers
	System_Einstellungen.PID_Regler_Geschwindigkeit_Max=5;			//Maximaler Ausgangswert des Geschwindigkeits-Reglers
	System_Einstellungen.Schrittmodus=0;	//Stepmodus der Motorregler
	System_Einstellungen.Platinenluefter_Sollwert=207;				//Sollwert für Platinenlüfter PWM
	System_Einstellungen.Gehaeuseluefter_Sollwert=207;				//Sollwert für Gehäuselüfter PWM
	System_Einstellungen.NeoPixelRing_Helligkeit=25;				//Helligkeit NeoPixelRing
	System_Einstellungen.MPU_Offset_AcelX=0;						//Offsetwert für MPU AcelX
	System_Einstellungen.MPU_Offset_AcelY=0;						//Offsetwert für MPU AcelY
	System_Einstellungen.MPU_Offset_AcelZ=0;						//Offsetwert für MPU AcelZ
	System_Einstellungen.MPU_Offset_GyroX=0;						//Offsetwert für MPU GyroX
	System_Einstellungen.MPU_Offset_GyroY=0;						//Offsetwert für MPU GyroY
	System_Einstellungen.MPU_Offset_GyroZ=0;						//Offsetwert für MPU GyroZ


}
void EEPROM_Werte_aktiveren(Balancing_Bot_Einstellungen EEPROM_Daten)
{
	Offset_acelx(EEPROM_Daten.MPU_Offset_AcelX);
	Offset_acely(EEPROM_Daten.MPU_Offset_AcelY);
	Offset_acelz(EEPROM_Daten.MPU_Offset_AcelZ);
	Offset_gyrox(EEPROM_Daten.MPU_Offset_GyroX);
	Offset_gyroy(EEPROM_Daten.MPU_Offset_GyroY);
	Offset_gyroz(EEPROM_Daten.MPU_Offset_GyroZ);

	PID_Regler_Winkel.SetTunings(EEPROM_Daten.PID_Regler_Winkel_P,EEPROM_Daten.PID_Regler_Winkel_I,EEPROM_Daten.PID_Regler_Winkel_D);
	PID_Regler_Winkel.SetOutputLimits(EEPROM_Daten.PID_Regler_Winkel_Min,EEPROM_Daten.PID_Regler_Winkel_Max);
	PID_Regler_Geschwindigkeit.SetTunings(EEPROM_Daten.PID_Regler_Geschwindigkeit_P,EEPROM_Daten.PID_Regler_Geschwindigkeit_I,EEPROM_Daten.PID_Regler_Geschwindigkeit_D);
	PID_Regler_Geschwindigkeit.SetOutputLimits(EEPROM_Daten.PID_Regler_Geschwindigkeit_Min,EEPROM_Daten.PID_Regler_Geschwindigkeit_Max);

	Motor_Links.StepMode_setzen(EEPROM_Daten.Schrittmodus);
	Motor_Rechts.StepMode_setzen(EEPROM_Daten.Schrittmodus);

	//NeoPixel_Setup(EEPROM_Daten.NeoPixelRing_Helligkeit);

	analogWrite(Pin_Platinenluefter,EEPROM_Daten.Platinenluefter_Sollwert);
	analogWrite(Pin_Gehaeuseluefter,EEPROM_Daten.Gehaeuseluefter_Sollwert);

}
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


	void test()
	{

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

	//Die Steuerbefehle für die Stepper direkt in das Ausgangsregister PORTB schreiben um die Motoren zeitgleich zu steuern.
	void Ausgangsregister_schreiben(bool MotorLinks_Step, bool MotorRechts_Step)
	{
		if (MotorLinks_Step==true && MotorRechts_Step==true)//beide einen Schritt
		{
			/*PORTB=PORTB|B01100000;
			delayMicroseconds(DRV8825_Step_Pause);
			PORTB=PORTB&B10011111;*/
			digitalWrite(Pin_Step_Rechts,true);
			digitalWrite(Pin_Step_Links,true);
			delayMicroseconds(2500);
			digitalWrite(Pin_Step_Rechts,false);
			digitalWrite(Pin_Step_Links,false);
		}
		else if (MotorLinks_Step==false && MotorRechts_Step==true)//Rechts einen Schritt
		{
			//PORTB=PORTB|B01000000;
			//delayMicroseconds(DRV8825_Step_Pause);
			//PORTB=PORTB&B10111111;
			digitalWrite(Pin_Step_Rechts,true);
			delayMicroseconds(2500);
			digitalWrite(Pin_Step_Rechts,false);
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
			/*PORTB=PORTB&B10011111;*/
			digitalWrite(Pin_Step_Rechts,false);
			digitalWrite(Pin_Step_Links,false);
		}

	}

	//Unterprogramm zur Motorensteuerung
	//Drehzahl ist benötigte Drehzahl für Balancing
	//Prozent_Rechts/Links entspricht dem Sollwert für den jewaligen Motor in Prozent 100% => Sollwert==Motorwert
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

	//Spannungsteilerberechnung für Spannungskontrolle
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

	//Spannungsbereich für die Akkumessungen berechnen Beispiel 8,4V-6V=2,4V
	double Akku_Messbereich_Berechnen(double AkkuMin, double AkkuMax)
	{
		double Messbereich =0.00;
		Messbereich= AkkuMax-AkkuMin;
		return Messbereich;
	}

	//Akkumessungen auswerten und in den Variablen die entsprechende Werte eintragen
	//Return TRUE Akkustatus in Ordnung FALSE Akkustatus niedrig oder kristisch
	//getestet Funktion i.O. am 19.10.17
	bool Akkuueberwachung (int AkkuPin1, int AkkuPin2)
	{
		if (Akku_Messbereich==0.00)
		{
			Akku_Messbereich=Akku_Messbereich_Berechnen(AkkuSpannungMin,AkkuSpannungMax);
		}

		//Akkumessung für akku 2 Akku GND auf A6
		//Akkumessung für akku 1 Akkugesammnt GND auf A7
		Akkuspannung2 = Spannungsteiler(SpannungsmessungR1,SpannungsmessungR2,AkkuPin1);
		AkkuSpannungGesamt = Spannungsteiler(SpannungsmessungR1,SpannungsmessungR2,AkkuPin2);
		Akkuspannung1 = AkkuSpannungGesamt-Akkuspannung2;
		Akku_Prozent = (((Akkuspannung1-AkkuSpannungMin)/Akku_Messbereich*100)+((Akkuspannung2-AkkuSpannungMin)/Akku_Messbereich*100))/2;



		if ((Akkuspannung1 <=  AkkuSpannungKritisch || Akkuspannung2 <= AkkuSpannungKritisch))
		{
			Status= 6;
			return false;

		}
	   else if (Akkuspannung1 <  AkkuSpannungNiedrig || Akkuspannung2 < AkkuSpannungNiedrig)
		{
			Status=5;
			return false;
		}
		else
		{
			if (Status==AkkuSpannungNiedrig||Status==AkkuSpannungKritisch)
			{
				Status=4;
			}
			return true;
		}
	}



	//Zeitmessung für 20ms Takt
	//Return TRUE wenn 20ms seit dem letzten Takt abgelaufen sind
	//getestet Funktion i.O. am 28.09.17
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
	//getestet Funktion i.O. am 28.09.17
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
	//getestet Funktion i.O. am 28.09.17
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
	//getestet Funktion i.O. am 02.08.17
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


#endif // !_ANDI_BIBILOTHEK_BALANCINGBOT_h