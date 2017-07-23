/*
 Name:		Messenger_Befehle.ino
 Created:	23.07.2017 16:57:37
 Author:	Andreas Bauer
*/
enum Befehle
{
	
	cmd_Statusmeldung,			//Statusmeldungen z.B.: Setup beendet
	cmd_Akkustand,				//Aktuelle Spannung des Primärakkus
	cmd_KalmanWinkel,			//Aktueller Winkel aus dem Kalman Filter
	cmd_RAW_Werte,				//RAW-Werte aus dem MPU ohne Bearbeitung
	cmd_Offset_Werte,			//Offset-Werte für die XYZ Achsen des MPU6050
	cmd_MPU_Kalibrieren,		//Automatik Kalibrierung durchführen
	
	cmd_PID_Winkel_MinMax,		//Winkel Regler Min und Max Werte setzen
	cmd_PID_Winkel_Sollwert		//Winkel Regler Sollwert setzen
};

enum Statusmeldungen
{
	Setup_beendet,				//Setup abgeschlossen

};

void Setup_cmdMessenger()
{
	cmdMessenger.attach(cmd_Statusmeldung,Statusmeldung);
}



/*------------CALLBACKS---------------*/

void Statusmeldung()
{

}

void Akkustand()
{

}

void KalmanWinkel()
{

}

void RAW_Werte()
{

}

void Offset_Werte()
{

}

void MPU_Kalibrieren()
{
	     
}

void PID_Winkel_MinMax()
{

}

void PID_Winkel_Sollwert()
{

}