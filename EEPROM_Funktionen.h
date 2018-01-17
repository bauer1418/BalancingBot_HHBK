// EEPROM_Funktionen.h

#ifndef _EEPROM_FUNKTIONEN_h
	#define _EEPROM_FUNKTIONEN_h
	#if defined(ARDUINO) && ARDUINO >= 100
		#include "arduino.h"
	#else
		#include "WProgram.h"
	#endif

	//Include Anweisungen
	#include <CmdMessenger.h>
	#include "Messenger_Enum.h"
	//#include "Andi_Bibilothek_BalancingBot.h"
	#include "MPU6050_BalancingBot_HHBK.h"
	#include <EEPROM.h>


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

	//Funktionsprotypen
	Balancing_Bot_Einstellungen Daten_aus_EEPROM_lesen();
	void EEPROM_Werte_aktiveren(Balancing_Bot_Einstellungen EEPROM_Daten);
	void Einstellungen_mit_Standart_Werten_beschreiben();
	void EEPROM_Werte_aktiveren(Balancing_Bot_Einstellungen EEPROM_Daten);


#endif //Ende der Define Anweisung