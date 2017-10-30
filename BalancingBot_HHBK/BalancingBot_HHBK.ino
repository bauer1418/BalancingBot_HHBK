/*
 Name:		BalancingBot_HHBK.ino
 Created:	19.07.2017 02:36:37
 Author:	Andreas Bauer
*/

//Benötigte Bibilotheken



#include <Adafruit_NeoPixel.h>



unsigned long Zykluszeit=0;				//akutelle Zykluszeit
bool MotorenEINAUS = false;				//Motoren Ein Aus Schalter 




 //the setup function runs once when you press reset or power the board
void setup() 
{
	Pin_Setup();

	analogWrite(Pin_Platinenluefter,207);//Platinenlüfter auf 80% stellen

	Setup_cmdMessenger();

	NeoPixel_Setup(25);

	if (MPUsetup==false)
	{
		Status=Fehler;
	}
	else
	{
		Status=Setup_beendet;
	}
	PID_Regler_Winkel.SetControllerDirection(DIRECT);
	PID_Regler_Winkel.SetMode(AUTOMATIC);
	PID_Regler_Winkel.SetOutputLimits(-120,120);//!!!!!!!!!!!!!!!!!!!!!!!!!!HIER IST DER FEHLER MIT PID AUSGANG NUR 0-255!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Sollwert_PID_Winkel=0.00;
	Motor_Links.StepMode_setzen(Stepper_Motor::Vollschritt);
	Motor_Rechts.StepMode_setzen(Stepper_Motor::Vollschritt);
	Statusmeldung();
}

 //the loop function runs over and over again until power down or reset
void loop() 
{
	Zykluszeit = Zykluszeit_Messung();
	Zeit_Takt_20ms();
	
	//Akkuueberwachung(Pin_Akku1_Messung,Pin_Akku2_Messung);
	//MPU Zyklus nur ausführen wenn MPU nicht gestört ist
	if (Fehlerspeicher!=MPU_NOT_FOUND && Fehlerspeicher!=MPU_READ_FAILED && Fehlerspeicher!=MPU_READ_TIMEOUT && Fehlerspeicher!=MPU_Write_FAILED)
	{
		MPU_Zyklus();
	}
	//else
	//{
	//	Fehlerspeicher=Kein_Fehler_vorhanden;
	//}
	Zyklusdaten_senden();

	if (MotorenEINAUS==true)
	{
		//Winkel auslesen
		//Eingang_PID_Winkel=GET_KalmanWinkelX();
		//PID-Regler ausführen
		//PID_Regler_Winkel.Compute();//PID-Regler für die Winkelsteuerung zyklisch ausführen
		//Motoren_Steuerung(Ausgang_PID_Winkel,100,100);
		
		Ausgangsregister_schreiben(Motor_Links.Step(100,100),Motor_Rechts.Step(100,100));
	}

	//Umkippschutz(20,GET_KalmanWinkelX());

	//PID_Regler_Geschwindigkeit.Compute();
	Akkuueberwachung(6,7);
	//Lueftersteuerung_Temperatur(GET_MPU_Temperatur(),Pin_Gehaeuseluefter);//Gehäuselüftersteuerung
	//cmdMessenger.sendCmd(cmd_Anzeige_Text,analogRead(Akkuspannung2));

	cmdMessenger.feedinSerialData();//cmdMessenger Datenauslesen und Callbacks auslösen
}
