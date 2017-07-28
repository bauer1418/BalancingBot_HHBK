/*
 Name:		Messenger_Befehle.ino
 Created:	23.07.2017 16:57:37
 Author:	Andreas Bauer
*/



enum Statusmeldungen
{
	Setup_beendet,				//Setup abgeschlossen

};

void Setup_cmdMessenger()
{
	cmdMessenger.attach(cmd_Statusmeldung,Statusmeldung);
	cmdMessenger.attach(cmd_Akkustand,Statusmeldung);
}



/*------------CALLBACKS---------------*/

void Statusmeldung()
{
	cmdMessenger.sendCmd(cmd_Statusmeldung,"Hallo Test!");
}

void Akkustand()
{
	cmdMessenger.sendCmdStart(cmd_Akkustand);	//Mehrfach Senden starten
	cmdMessenger.sendCmdArg(15.0);				//Argument
	cmdMessenger.sendCmdEnd();					//Senden beenden
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