/*
 Name:		Messenger_Enum.h
 Created:	01.09.2017 04:58:17
 Author:	Andreas Bauer
*/


enum Befehle
{
	
	cmd_Statusmeldung,			//Statusmeldungen z.B.: Setup beendet
	cmd_Fehlermeldung,			//Fehlermeldungen ausgeben
	cmd_Akkustand_Prozent,		//Aktuelle Spannung des Akkupacks in %
	cmd_KalmanWinkel,			//Aktueller Winkel aus dem Kalman Filter
	//cmd_RAW_Werte,				//RAW-Werte aus dem MPU ohne Bearbeitung
	cmd_Offset_Werte,			//Offset-Werte f�r die XYZ Achsen des MPU6050
	cmd_MPU_Kalibrieren,		//Automatik Kalibrierung durchf�hren
	cmd_PID_Winkel_MinMax,		//Winkel Regler Min und Max Werte setzen
	cmd_PID_Winkel_Sollwert,	//Winkel Regler Sollwert setzen
	cmd_Umkipperkennung_quitt,	//Umkipperkennung zur�cksetzen
	cmd_MPU_Temperatur,			//MPU Temperatur senden
	cmd_MotorenEINAUS,			//Motoren Ein(true) oder Aus(false) schalten
	cmd_Anzeige_Text,			//Anzuzeigender Text aus dem Arduino
	cmd_Zyklusdaten,			//Zyklische Systemdaten f�r Steuerung senden
	cmd_P_I_D_Werte,			//Parameter 1 steht f�r welchen Regler 1=Winkel PID Werte verstellen
	

};

enum Statusmeldungen
{
	System_Start,				//Systemstart wird durchgef�hrt
	MPU_Start,					//MPU Systemstart
	MPU_Setup_OK,				//MPU Setup i.O.
	Setup_beendet,				//Setup abgeschlossen
	System_Bereit,				//System ist f�r den Balancing Betrieb bereit
	Akkustand_niedrig,			//Akkustand unter 25% 
	Akku_kritisch,				//Akkustand unter 10% Balancing Betrieb gesperrt!
	Umkipperkennung_ausgeloest, //Umkipperkennung hat angesprochen Winkel gr��er als 25�
	Fehler,						//System ist gest�rt siehe Fehlermeldungen

};

enum Fehlermeldungen
{
	Kein_Fehler_vorhanden,		//Kein Fehler aktuell vorhanden
	Motortreiber_Rechts,		//Motortreiber f�r Motor Rechts gest�rt
	Motortreiber_Links,			//Motortreiber f�r Motor Links gest�rt
	Akku1_Messung,				//Akkumessung Akku1 au�er Toleranz
	Akku2_Messung,				//Akkumessung Akku2 au�er Toleranz
	MPU_READ_TIMEOUT,			//MPU Fehler Komunikation hat Read TIMEOUT
	MPU_Write_FAILED,			//MPU Fehler Daten Schreiben war nicht m�glich
	MPU_READ_FAILED,			//MPU Fehler Daten Lesen war nicht m�glich
	MPU_NOT_FOUND,				//MPU Fehler MPU WHO IAM Register konnte nicht gelesen werden
};




