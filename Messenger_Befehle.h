/*
 Name:		Messenger_Befehle.ino
 Created:	23.07.2017 16:57:37
 Author:	Andreas Bauer
*/


#ifndef _MESSENGER_BEFEHLE_h
	#define _MESSENGER_BEFEHLE_h

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "arduino.h"
	#else
		#include "WProgram.h"
	#endif
	//Include Anweisungen
	#include <CmdMessenger.h>
	//#include "Messenger_Enum.h"
	//#include "Andi_Bibilothek_BalancingBot.h"
	//#include "MPU6050_BalancingBot_HHBK.h"
	//#include "EEPROM_Funktionen.h"

	//Variablen

	CmdMessenger cmdMessenger = CmdMessenger(Serial);


	//Setup Routine für die Serielle Komunikation mit dem cmdMessenger Library Arduino<->Steuerungssoftware
	void Setup_cmdMessenger();

	void Zyklusdaten_senden();

	void Statusmeldung();

	void Fehlermeldung();

	void Akkustand_Prozent();

	void Akku_Volt();

	void KalmanWinkel();

	void Offset_Werte();

	void Offset_Werte_senden(byte Befehlsnummer);

	void MPU_Kalibrieren();

	void PID_Winkel_MinMax();

	void PID_Winkel_Sollwert();

	void MPU_Temperatur();

	void MotorenSchalten();

	void P_I_D_Werte();

	void Daten_aus_EEPROM_senden();

	void Daten_ins_EEPROM_speichern();

#endif // !_MPU6050_BALANCINGBOT_HHBK_h