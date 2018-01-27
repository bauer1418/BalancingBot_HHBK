/*
 Name:		Messenger_Befehle.ino
 Created:	23.07.2017 16:57:37
 Author:	Andreas Bauer
*/

#ifndef _Messenger_Befehle
	#define _Messenger_Befehle

	//Funktionsprototypen
	void Zyklusdaten_senden();
	void Statusmeldung();
	void Fehlermeldung();
	void Akkustand_Prozent();
	void KalmanWinkel();
	void Offset_Werte();
	void Offset_Werte_senden(byte Befehlsnummer);
	void MPU_Kalibrieren();
	void PID_Winkel_MinMax();
	void PID_Winkel_Sollwert();
	void MPU_Temperatur();
	void MotorenSchalten();
	void P_I_D_Werte();
	void Setup_cmdMessenger();
	void Akkus_Spannungen_senden();
	
	//Include-Anweisungen
	#include "Andi_Bibilothek_BalancingBot.h"
	#include "Messenger_Enum.h"
	#include "MPU6050_BalancingBot_HHBK.h"

	//Variablen
	

	/*------------CALLBACKS---------------*/

	//Zyklusdaten alle 20ms senden
	void Zyklusdaten_senden()
	{
		if (Allgemeine_Zeitfunktion.ZeitTakt_20ms()==true)
		{
			cmdMessenger.sendCmdStart(cmd_Zyklusdaten);						//Mehrfach Senden starten
			cmdMessenger.sendCmdArg(GET_KalmanWinkelY());					//Aktueller KalmanWinkel senden
			cmdMessenger.sendCmdArg(Ausgang_PID_Winkel);					//PID Winkel Ausgangswert				
			cmdMessenger.sendCmdArg(MotorenEINAUS);							//Aktueller MotorEINAUS Status senden
			cmdMessenger.sendCmdArg(Zykluszeit);							//Zykluszeit senden
			cmdMessenger.sendCmdArg(Status);								//Aktuellen Systemstatus senden
			cmdMessenger.sendCmdArg(PID_Regler_Winkel.Get_PID_P_Wert());	//P-Wert senden Winkel
			cmdMessenger.sendCmdArg(PID_Regler_Winkel.Get_PID_I_Wert());	//I-Wert senden Winkel
			cmdMessenger.sendCmdArg(PID_Regler_Winkel.Get_PID_D_Wert());	//D-Wert senden Winkel
			cmdMessenger.sendCmdArg(Sollwert_PID_Winkel);					//Sollwert für Winkelregler senden
			cmdMessenger.sendCmdEnd();										//Senden beenden
		}
		//Typische Zyklusdaten Kalmanwinkel PID Ausgang P I D Teile MotorEINAUS Zykluszeit
	}

	void EEPROM_lesen()
	{
		System_Einstellungen=Daten_aus_EEPROM_lesen();
		EEPROM_Werte_aktiveren(System_Einstellungen);
		cmdMessenger.sendCmd(cmd_Einstellungen_aus_EEPROM_lesen,System_Einstellungen.Werte_ueberschrieben);
	}
	void EEPROM_speichern()
	{
		System_Einstellungen.Werte_ueberschrieben = 5;
		EEPROM.put(0,System_Einstellungen);
		cmdMessenger.sendCmd(cmd_Einstellungen_ins_EEPROM_speichern,F("Daten gespeichert"));
	}
	//Aktuellen Systemstatus senden
	void Statusmeldung()
	{
		cmdMessenger.sendCmd(cmd_Statusmeldung,Status);
	}

	//Aktuellen Fehler senden
	void Fehlermeldung()
	{
		cmdMessenger.sendCmd(cmd_Fehlermeldung,Fehlerspeicher);
		
	}
	void Akkustand_Prozent()
	{
		cmdMessenger.sendCmd(cmd_Akkustand_Prozent,Akku_Prozent);
	}

	void KalmanWinkel()
	{
		cmdMessenger.sendCmd(cmd_KalmanWinkel,GET_KalmanWinkelX());
	}

	//void RAW_Werte()
	//{
	//
	//}

	void Offset_Werte()
	{
		//Reihenfolge der Offset Werte aus dem cmdMessenger 1.acelx 2.acely 3.acelz 4.gyrox 5.gyroy 6.gyroz
		//Als Antwort werden die neuen Übernommenen Werte zurückgesendet
		Offset_acelx(cmdMessenger.readDoubleArg());
		Offset_acely(cmdMessenger.readDoubleArg());
		Offset_acelz(cmdMessenger.readDoubleArg());
		Offset_gyrox(cmdMessenger.readDoubleArg());
		Offset_gyroy(cmdMessenger.readDoubleArg());
		Offset_gyroz(cmdMessenger.readDoubleArg());
		//Antwort senden
		Offset_Werte_senden(cmd_Offset_Werte);
	
	}
	void Offset_Werte_senden(byte Befehlsnummer)
	{
		cmdMessenger.sendCmdStart(Befehlsnummer);			//Mehrfach Senden starten
		cmdMessenger.sendCmdArg(Offset_acelx());			//Offset Wert für acelx senden
		cmdMessenger.sendCmdArg(Offset_acely());			//Offset Wert für acely senden
		cmdMessenger.sendCmdArg(Offset_acelz());			//Offset Wert für acelz senden
		cmdMessenger.sendCmdArg(Offset_gyrox());			//Offset Wert für gyrox senden
		cmdMessenger.sendCmdArg(Offset_gyroy());			//Offset Wert für gyroy senden
		cmdMessenger.sendCmdArg(Offset_gyroz());			//Offset Wert für gyroz senden
		cmdMessenger.sendCmdEnd();							//Senden beenden
	}

	void MPU_Kalibrieren()
	{
		MotorenEINAUS=false;   

		Motoren_EINAUS_Schalten(MotorenEINAUS);

		cmdMessenger.sendCmd(cmd_Anzeige_Text,F("MPU Kalibrierung"));
	
		cmdMessenger.sendCmd(cmd_Anzeige_Text,F("Bitte Sensor nicht bewegen!"));
	
		cmdMessenger.sendCmd(cmd_Anzeige_Text,F("Kalibierung startet in 5sec!"));
	
		delay(5000);
	
		MPU6050_Kalibrieren();
	
		Offset_Werte_senden(cmd_MPU_Kalibrieren);

	}

	void PID_Winkel_MinMax()
	{
		double min=0, max=0;
		min=cmdMessenger.readDoubleArg();
		max=cmdMessenger.readDoubleArg();
		PID_Regler_Winkel.SetOutputLimits(min, max);
		cmdMessenger.sendCmdStart(cmd_PID_Winkel_MinMax);
		cmdMessenger.sendCmdArg(min);
		cmdMessenger.sendCmdArg(max);
		cmdMessenger.sendCmdEnd();
	}

	void PID_Winkel_Sollwert()
	{
		Sollwert_Steuerung=cmdMessenger.readDoubleArg();
		//Sollwert_PID_Winkel=cmdMessenger.readDoubleArg();
		cmdMessenger.sendCmd(cmd_PID_Winkel_Sollwert, Sollwert_Steuerung);
	}
	void MPU_Temperatur()
	{
		cmdMessenger.sendCmd(cmd_MPU_Temperatur,GET_MPU_Temperatur());
	}
	void MotorenSchalten()
	{
		MotorenEINAUS=cmdMessenger.readBoolArg();
		Motoren_EINAUS_Schalten(MotorenEINAUS);
		cmdMessenger.sendCmd(cmd_MotorenEINAUS,MotorenEINAUS);
	}
	void P_I_D_Werte()
	{
		byte Regler = cmdMessenger.readInt16Arg();//1=Winkelregler ändern 2=Speedregler ändern 3=Winkelregler senden 4=Speedregler senden
		double P=0,I=0,D=0;
		P=cmdMessenger.readDoubleArg();
		I=cmdMessenger.readDoubleArg();
		D=cmdMessenger.readDoubleArg();


		if (Regler== 1)//Neue PID Werte für den Winkelregler festlegen und als Antwort zurücksenden
		{
			//PID_Regler_Winkel.SetTunings(P,I,D);
			System_Einstellungen.PID_Regler_Winkel_Nah_P=P;
			System_Einstellungen.PID_Regler_Winkel_Nah_I=I;
			System_Einstellungen.PID_Regler_Winkel_Nah_D=D;
			PID_Regler_Winkel.SetTunings(System_Einstellungen.PID_Regler_Winkel_Nah_P, System_Einstellungen.PID_Regler_Winkel_Nah_I, System_Einstellungen.PID_Regler_Winkel_Nah_D);
			cmdMessenger.sendCmdStart(cmd_P_I_D_Werte);
			cmdMessenger.sendCmdArg(1);/*
			cmdMessenger.sendCmdArg(System_Einstellungen.PID_Regler_Winkel_Nah_P);
			cmdMessenger.sendCmdArg(System_Einstellungen.PID_Regler_Winkel_Nah_I);
			cmdMessenger.sendCmdArg(System_Einstellungen.PID_Regler_Winkel_Nah_D);*/
			cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKp());
			cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKi());
			cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKd());

		}
		else if (Regler== 2)//Neue PID Werte für den Geschwindigkeitsregler festlegen und als Antwort zurücksenden
		{
			PID_Regler_Geschwindigkeit.SetTunings(P,I,D);
			System_Einstellungen.PID_Regler_Geschwindigkeit_P=P;
			System_Einstellungen.PID_Regler_Geschwindigkeit_I=I;
			System_Einstellungen.PID_Regler_Geschwindigkeit_D=D;
			cmdMessenger.sendCmdStart(cmd_P_I_D_Werte);
			cmdMessenger.sendCmdArg(2);
			cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKp());
			cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKi());
			cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKd());

		}
			else if (Regler== 3)//Nur Anzeige der PID Werte für Winkelregler
		{
			cmdMessenger.sendCmdStart(cmd_P_I_D_Werte);
			cmdMessenger.sendCmdArg(1);
			cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKp());
			cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKi());
			cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKd());

		}
			else if (Regler== 4)//Nur Anzeige der PID Werte für Geschwindigkeitsregler
		{
			cmdMessenger.sendCmdStart(cmd_P_I_D_Werte);
			cmdMessenger.sendCmdArg(2);
			cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKp());
			cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKi());
			cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKd());

		}
			else if (Regler == 5)//Einstellung der Fernwerte für den Winkel Regler
			{

				//PID_Regler_Winkel.SetTunings(P, I, D);
				System_Einstellungen.PID_Regler_Winkel_Fern_P = P;
				System_Einstellungen.PID_Regler_Winkel_Fern_I = I;
				System_Einstellungen.PID_Regler_Winkel_Fern_D = D;
				cmdMessenger.sendCmdStart(cmd_P_I_D_Werte);
				cmdMessenger.sendCmdArg(1);
				/*
				cmdMessenger.sendCmdArg(System_Einstellungen.PID_Regler_Winkel_Nah_P);
				cmdMessenger.sendCmdArg(System_Einstellungen.PID_Regler_Winkel_Nah_I);
				cmdMessenger.sendCmdArg(System_Einstellungen.PID_Regler_Winkel_Nah_D);*/
				cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKp());
				cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKi());
				cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKd());
			}
			else if (Regler == 6)//Nur Anzeige der PID Werte für Geschwindigkeitsregler
			{
			}
	}

	void Akkus_Spannungen_senden()
	{
		cmdMessenger.sendCmdStart(cmd_Akku_Spannungen_senden);
		cmdMessenger.sendCmdArg(Akkuspannung1);
		cmdMessenger.sendCmdArg(Akkuspannung2);
		cmdMessenger.sendCmdArg(AkkuSpannungGesamt);
		cmdMessenger.sendCmdEnd();
	}

	//Sampletimeeinstellung der PID Regler 1=Winkel 2=Speed
	void PID_Sampletime()
	{
		byte Regler = cmdMessenger.readInt16Arg();//1=Winkelregler ändern 2=Speedregler ändern
		if (Regler==1)
		{
			System_Einstellungen.PID_Winkel_Sampletime = cmdMessenger.readInt16Arg();
			PID_Regler_Winkel.SetSampleTime(System_Einstellungen.PID_Winkel_Sampletime);
		}
		if (Regler == 2)
		{
			System_Einstellungen.PID_Geschwindigkeit_Sampletime = cmdMessenger.readInt16Arg();
			PID_Regler_Geschwindigkeit.SetSampleTime(System_Einstellungen.PID_Geschwindigkeit_Sampletime);
		}

	}
	//Setup Routine für die Serielle Komunikation mit dem cmdMessenger Library Arduino<->Steuerungssoftware
	void Setup_cmdMessenger()
	{
		Serial.begin(57600);
		//Funktionsaufruf bei cmd_Signal
		cmdMessenger.attach(cmd_Statusmeldung,Statusmeldung);
		cmdMessenger.attach(cmd_Fehlermeldung,Fehlermeldung);
		cmdMessenger.attach(cmd_Akkustand_Prozent,Akkustand_Prozent);
		cmdMessenger.attach(cmd_KalmanWinkel,KalmanWinkel);
		//cmdMessenger.attach(cmd_RAW_Werte,RAW_Werte);
		cmdMessenger.attach(cmd_Offset_Werte,Offset_Werte);
		cmdMessenger.attach(cmd_MPU_Temperatur,MPU_Temperatur);
		cmdMessenger.attach(cmd_MotorenEINAUS,MotorenSchalten);
		cmdMessenger.attach(cmd_MPU_Kalibrieren,MPU_Kalibrieren);
		cmdMessenger.attach(cmd_PID_Winkel_MinMax,PID_Winkel_MinMax);
		cmdMessenger.attach(cmd_PID_Winkel_Sollwert,PID_Winkel_Sollwert);
		cmdMessenger.attach(cmd_P_I_D_Werte,P_I_D_Werte);
		cmdMessenger.attach(cmd_Einstellungen_aus_EEPROM_lesen,EEPROM_lesen);
		cmdMessenger.attach(cmd_Einstellungen_ins_EEPROM_speichern,EEPROM_speichern);
		cmdMessenger.attach(cmd_Akku_Spannungen_senden,Akkus_Spannungen_senden);
		cmdMessenger.attach(cmd_PID_Sampletime, PID_Sampletime);
		cmdMessenger.printLfCr();//Nach jeder Message eine neue Zeile beginnen
	}
#endif // !_Messenger_Befehle