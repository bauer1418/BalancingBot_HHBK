/*
 Name:		Messenger_Befehle.ino
 Created:	23.07.2017 16:57:37
 Author:	Andreas Bauer
*/

#include "Messenger_Befehle.h"
#include "EEPROM_Funktionen.h"
#include "Andi_Bibilothek_BalancingBot.h"
#include"TEST.h"
//Variablen



//Setup Routine f�r die Serielle Komunikation mit dem cmdMessenger Library Arduino<->Steuerungssoftware
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
	cmdMessenger.attach(cmd_Fahrbefehl_Vor,P_I_D_Werte);
	cmdMessenger.attach(cmd_Fahrbefehl_Zurueck,P_I_D_Werte);
	cmdMessenger.attach(cmd_Fahrbefehl_Kurve,P_I_D_Werte);
	cmdMessenger.attach(cmd_Einstellungen_aus_EEPROM_lesen,Daten_aus_EEPROM_senden);
	cmdMessenger.attach(cmd_Einstellungen_ins_EEPROM_speichern,Daten_ins_EEPROM_speichern);
	cmdMessenger.attach(cmd_Akku_Spannungen_senden,Akku_Volt);
	cmdMessenger.printLfCr();//Nach jeder Message eine neue Zeile beginnen
}



/*------------CALLBACKS---------------*/

//Zyklusdaten alle 20ms senden
void Zyklusdaten_senden()
{
	if (Zeit_Takt_20ms()==true)
	{
		cmdMessenger.sendCmdStart(cmd_Zyklusdaten);			//Mehrfach Senden starten
		cmdMessenger.sendCmdArg(GET_KalmanWinkelY());		//Aktueller KalmanWinkel senden
		cmdMessenger.sendCmdArg(Ausgang_PID_Winkel);		//PID Winkel Ausgangswert				
		cmdMessenger.sendCmdArg(MotorenEINAUS);				//Aktueller MotorEINAUS Status senden
		cmdMessenger.sendCmdArg(Zykluszeit);				//Zykluszeit senden
		cmdMessenger.sendCmdArg(Status);					//Aktuellen Systemstatus senden
		cmdMessenger.sendCmdEnd();							//Senden beenden
	}
	//Typische Zyklusdaten Kalmanwinkel PID Ausgang P I D Teile MotorEINAUS Zykluszeit
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
void Akku_Volt()
{
	cmdMessenger.sendCmdStart(cmd_Akku_Spannungen_senden);			//Mehrfach Senden starten
	cmdMessenger.sendCmdArg(Akkuspannung1);			
	cmdMessenger.sendCmdArg(Akkuspannung2);
	cmdMessenger.sendCmdEnd();
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
	//Als Antwort werden die neuen �bernommenen Werte zur�ckgesendet
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
	cmdMessenger.sendCmdArg(Offset_acelx());			//Offset Wert f�r acelx senden
	cmdMessenger.sendCmdArg(Offset_acely());			//Offset Wert f�r acely senden
	cmdMessenger.sendCmdArg(Offset_acelz());			//Offset Wert f�r acelz senden
	cmdMessenger.sendCmdArg(Offset_gyrox());			//Offset Wert f�r gyrox senden
	cmdMessenger.sendCmdArg(Offset_gyroy());			//Offset Wert f�r gyroy senden
	cmdMessenger.sendCmdArg(Offset_gyroz());			//Offset Wert f�r gyroz senden
	cmdMessenger.sendCmdEnd();							//Senden beenden
}

void MPU_Kalibrieren()
{
	MotorenEINAUS=false;   

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
	Sollwert_PID_Winkel=cmdMessenger.readDoubleArg();
	cmdMessenger.sendCmd(cmd_PID_Winkel_Sollwert,Sollwert_PID_Winkel);
}
void MPU_Temperatur()
{
	cmdMessenger.sendCmd(cmd_MPU_Temperatur,GET_MPU_Temperatur());
}
void MotorenSchalten()
{
	MotorenEINAUS=cmdMessenger.readBoolArg();
	if (MotorenEINAUS==true)
	{
		Motor_Links.Aktiv_Schalten(true);
		Motor_Rechts.Aktiv_Schalten(true);
	}
	else
	{
		Motor_Links.Aktiv_Schalten(false);
		Motor_Rechts.Aktiv_Schalten(false);
	}
	cmdMessenger.sendCmd(cmd_MotorenEINAUS,MotorenEINAUS);
}
void P_I_D_Werte()
{
	byte Regler = cmdMessenger.readInt16Arg();//1=Winkelregler �ndern 2=Speedregler �ndern 3=Winkelregler senden 4=Speedregler senden
	double P=0,I=0,D=0;
	P=cmdMessenger.readDoubleArg();
	I=cmdMessenger.readDoubleArg();
	D=cmdMessenger.readDoubleArg();


	if (Regler== 1)//Neue PID Werte f�r den Winkelregler festlegen und als Antwort zur�cksenden
	{
		PID_Regler_Winkel.SetTunings(P,I,D);
		cmdMessenger.sendCmdStart(cmd_P_I_D_Werte);
		cmdMessenger.sendCmdArg(1);
		cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKp());
		cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKi());
		cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKd());

	}
	else if (Regler== 2)//Neue PID Werte f�r den Geschwindigkeitsregler festlegen und als Antwort zur�cksenden
	{
		PID_Regler_Geschwindigkeit.SetTunings(P,I,D);
		cmdMessenger.sendCmdStart(cmd_P_I_D_Werte);
		cmdMessenger.sendCmdArg(2);
		cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKp());
		cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKi());
		cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKd());

	}
		else if (Regler== 3)//Nur Anzeige der PID Werte f�r Winkelregler
	{
		cmdMessenger.sendCmdStart(cmd_P_I_D_Werte);
		cmdMessenger.sendCmdArg(1);
		cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKp());
		cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKi());
		cmdMessenger.sendCmdArg(PID_Regler_Winkel.GetKd());

	}
		else if (Regler== 4)//Nur Anzeige der PID Werte f�r Geschwindigkeitsregler
	{
		cmdMessenger.sendCmdArg(2);
		cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKp());
		cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKi());
		cmdMessenger.sendCmdArg(PID_Regler_Geschwindigkeit.GetKd());

	}
}
void Daten_aus_EEPROM_senden()
{
	Balancing_Bot_Einstellungen EEPROM_Daten;
	EEPROM_Daten=Daten_aus_EEPROM_lesen();
	cmdMessenger.sendCmdStart(cmd_Einstellungen_aus_EEPROM_lesen);
	cmdMessenger.sendCmdArg(EEPROM_Daten.Werte_ueberschrieben);
	cmdMessenger.sendCmdArg(EEPROM_Daten.PID_Regler_Winkel_P);
	cmdMessenger.sendCmdArg(EEPROM_Daten.PID_Regler_Winkel_I);
	cmdMessenger.sendCmdArg(EEPROM_Daten.PID_Regler_Winkel_D);
	cmdMessenger.sendCmdArg(EEPROM_Daten.PID_Regler_Geschwindigkeit_P);
	cmdMessenger.sendCmdArg(EEPROM_Daten.PID_Regler_Geschwindigkeit_I);
	cmdMessenger.sendCmdArg(EEPROM_Daten.PID_Regler_Geschwindigkeit_D);
	cmdMessenger.sendCmdArg(EEPROM_Daten.PID_Regler_Winkel_Min);
	cmdMessenger.sendCmdArg(EEPROM_Daten.PID_Regler_Winkel_Max);
	cmdMessenger.sendCmdArg(EEPROM_Daten.PID_Regler_Geschwindigkeit_Min);
	cmdMessenger.sendCmdArg(EEPROM_Daten.PID_Regler_Geschwindigkeit_Max);
	cmdMessenger.sendCmdArg(EEPROM_Daten.Schrittmodus);
	cmdMessenger.sendCmdArg(EEPROM_Daten.Platinenluefter_Sollwert);
	cmdMessenger.sendCmdArg(EEPROM_Daten.Gehaeuseluefter_Sollwert);
	cmdMessenger.sendCmdArg(EEPROM_Daten.NeoPixelRing_Helligkeit);

}
void Daten_ins_EEPROM_speichern()
{
	EEPROM.put(0,System_Einstellungen);
}