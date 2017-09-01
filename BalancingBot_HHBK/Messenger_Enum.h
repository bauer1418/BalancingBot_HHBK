/*
 Name:		Messenger_Enum.h
 Created:	01.09.2017 04:58:17
 Author:	Andreas Bauer
*/


enum Befehle
{
	
	cmd_Statusmeldung,			//Statusmeldungen z.B.: Setup beendet
	cmd_Fehlermeldung,			//Fehlermeldungen ausgeben
	cmd_Akkustand_Prozent,				//Aktuelle Spannung des Akkupacks in %
	cmd_KalmanWinkel,			//Aktueller Winkel aus dem Kalman Filter
	cmd_RAW_Werte,				//RAW-Werte aus dem MPU ohne Bearbeitung
	cmd_Offset_Werte,			//Offset-Werte für die XYZ Achsen des MPU6050
	cmd_MPU_Kalibrieren,		//Automatik Kalibrierung durchführen
	
	cmd_PID_Winkel_MinMax,		//Winkel Regler Min und Max Werte setzen
	cmd_PID_Winkel_Sollwert,	//Winkel Regler Sollwert setzen
};

enum Statusmeldungen
{
	Setup_beendet,				//Setup abgeschlossen
	System_Bereit,				//System ist für den Balancing Betrieb bereit
	Akkustand_niedrig,			//Akkustand unter 25% 
	Akku_kritisch,				//Akkustand unter 10% Balancing Betrieb gesperrt!
	Umkipperkennung_ausgeloest, //Umkipperkennung hat angesprochen Winkel größer als 25°

};

enum Fehlermeldungen
{
	Kein_Fehler_vorhanden,		//Kein Fehler aktuell vorhanden
	MPU_Fehler,					//MPU gestört
	Motortreiber_Rechts,		//Motortreiber für Motor Rechts gestört
	Motortreiber_Links,			//Motortreiber für Motor Links gestört
	Akku1_Messung,				//Akkumessung Akku1 außer Toleranz
	Akku2_Messung,				//Akkumessung Akku2 außer Toleranz

};




