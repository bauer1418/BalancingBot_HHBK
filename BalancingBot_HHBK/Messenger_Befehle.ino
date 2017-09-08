/*
 Name:		Messenger_Befehle.ino
 Created:	23.07.2017 16:57:37
 Author:	Andreas Bauer
*/

//Variablen



//Setup Routine für die Serielle Komunikation mit dem cmdMessenger Library Arduino<->Steuerungssoftware
void Setup_cmdMessenger()
{
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
	cmdMessenger.printLfCr();//Nach jeder Message eine neue Zeile beginnen
}



/*------------CALLBACKS---------------*/

//Zyklusdaten alle 1s senden
void Zyklusdaten_senden()
{
	if (Zeit_Takt_20ms()==true)
	{
		cmdMessenger.sendCmdStart(cmd_Zyklusdaten);	//Mehrfach Senden starten
		cmdMessenger.sendCmdArg(GET_KalmanWinkelX());				//Zykluszeit
		cmdMessenger.sendCmdArg(Ausgang_PID_Winkel);				
		/*cmdMessenger.sendCmdArg();
		cmdMessenger.sendCmdArg(Zykluszeit);				
		cmdMessenger.sendCmdArg(Zykluszeit);*/				
		cmdMessenger.sendCmdArg(MotorenEINAUS);				
		cmdMessenger.sendCmdArg(Zykluszeit);				
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
	cmdMessenger.sendCmdStart(cmd_Anzeige_Text);
	cmdMessenger.sendCmd(cmd_Anzeige_Text,F("MPU Kalibrierung"));
	//cmdMessenger.sendCmdArg(F("MPU Kalibrierung"));
	cmdMessenger.sendCmd(cmd_Anzeige_Text,F("Bitte Sensor nicht bewegen!"));
	//cmdMessenger.sendCmdArg(F("Bitte Sensor nicht bewegen!"));
	cmdMessenger.sendCmd(cmd_Anzeige_Text,F("Kalibierung startet in 5sec!"));
	//cmdMessenger.sendCmdArg(F("Kalibierung startet in 5sec!"));

	delay(5000);
	//cmdMessenger.sendCmdArg(F("Kalibierung aktiv"));
	cmdMessenger.sendCmd(cmd_Anzeige_Text,F("Kalibierung aktiv"));
	MPU6050_Kalibrieren();
	//cmdMessenger.sendCmdEnd();
	Offset_Werte_senden(cmd_MPU_Kalibrieren);	
}

void PID_Winkel_MinMax()
{

}

void PID_Winkel_Sollwert()
{

}
void MPU_Temperatur()
{
	cmdMessenger.sendCmd(cmd_MPU_Temperatur,GET_MPU_Temperatur());
}
void MotorenSchalten()
{
	MotorenEINAUS=cmdMessenger.readBoolArg();
	cmdMessenger.sendCmd(cmd_MotorenEINAUS,MotorenEINAUS);
}