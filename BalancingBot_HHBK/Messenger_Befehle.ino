/*
 Name:		Messenger_Befehle.ino
 Created:	23.07.2017 16:57:37
 Author:	Andreas Bauer
*/




void Setup_cmdMessenger()
{
	cmdMessenger.attach(cmd_Statusmeldung,Statusmeldung);
	cmdMessenger.attach(cmd_Fehlermeldung,Statusmeldung);
	cmdMessenger.attach(cmd_Akkustand_Prozent,Akkustand_Prozent);
	cmdMessenger.attach(cmd_RAW_Werte,RAW_Werte);
}



/*------------CALLBACKS---------------*/

void Statusmeldung()
{
	cmdMessenger.sendCmd(cmd_Statusmeldung,"Hallo Test!");
}

void Akkustand_Prozent()
{
	cmdMessenger.sendCmdStart(cmd_Akkustand_Prozent);	//Mehrfach Senden starten
	cmdMessenger.sendCmdArg(15.0F);				//Argument
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