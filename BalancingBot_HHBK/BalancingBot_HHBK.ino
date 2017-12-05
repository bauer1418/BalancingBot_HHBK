/*
 Name:		BalancingBot_HHBK.ino
 Created:	19.07.2017 02:36:37
 Author:	Andreas Bauer
*/


unsigned long Zykluszeit=0;				//akutelle Zykluszeit
bool volatile MotorenEINAUS = false;				//Motoren Ein Aus Schalter 
volatile double Setpoint_bias = 0.0;

//Ben�tigte Bibilotheken
#include <Adafruit_NeoPixel.h>
#include "Andi_Bibilothek_BalancingBot.h"
#include "MPU6050_BalancingBot_HHBK.h"
#include "Messenger_Befehle.h"
#include "Andi_ZeitTakt_Funktionen.h"


 //the setup function runs once when you press reset or power the board
void setup() 
{
	Interrupt_Setup();

	Pin_Setup();

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
	PID_Regler_Winkel.SetSampleTime(System_Einstellungen.PID_Winkel_Sampletime);
	PID_Regler_Winkel.SetOutputLimits(-100,100);//!!!!!!!!!!!!!!!!!!!!!!!!!!HIER IST DER FEHLER MIT PID AUSGANG NUR 0-255!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Sollwert_PID_Winkel=0.00;
	Motor_Links.StepMode_setzen(Stepper_Motor::Achtelschritt);
	Motor_Rechts.StepMode_setzen(Stepper_Motor::Achtelschritt);
	Statusmeldung();
}

 //the loop function runs over and over again until power down or reset
void loop() 
{
	Zykluszeit = Zykluszeit_Messung();
	Allgemeine_Zeitfunktion.ZeitTakt();
	
	//Akkuueberwachung(Pin_Akku1_Messung,Pin_Akku2_Messung);
	//MPU Zyklus nur ausf�hren wenn MPU nicht gest�rt ist
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
		analogWrite(Pin_Platinenluefter,80*255/100);

		//Winkel auslesen
		Eingang_PID_Winkel=GET_KalmanWinkelY();
		//if (Eingang_PID_Winkel>5+Sollwert_PID_Winkel || Eingang_PID_Winkel< Sollwert_PID_Winkel-5)
		//{
		////	PID_Regler_Winkel.SetTunings(System_Einstellungen.PID_Regler_Winkel_Fern_P, System_Einstellungen.PID_Regler_Winkel_Fern_I, System_Einstellungen.PID_Regler_Winkel_Fern_D);
		//	Motor_Links.StepMode_setzen(Stepper_Motor::Halbschritt);
		//	Motor_Rechts.StepMode_setzen(Stepper_Motor::Halbschritt);
		//}
		//else
		//{
		//	//PID_Regler_Winkel.SetTunings(System_Einstellungen.PID_Regler_Winkel_Nah_P, System_Einstellungen.PID_Regler_Winkel_Nah_I, System_Einstellungen.PID_Regler_Winkel_Nah_D);
		//	Motor_Links.StepMode_setzen(Stepper_Motor::Viertelschritt);
		//	Motor_Rechts.StepMode_setzen(Stepper_Motor::Viertelschritt);
		/*}*/
		//PID-Regler ausf�hren



		Sollwert_PID_Winkel = Sollwert_Steuerung + Setpoint_bias;
		
		PID_Regler_Winkel.Compute();//PID-Regler f�r die Winkelsteuerung zyklisch ausf�hren
		Motor_Rechts.Drehzahl_festlegen(Ausgang_PID_Winkel, 100);
		Motor_Links.Drehzahl_festlegen(Ausgang_PID_Winkel, 100);
		/*if (Sollwert_Steuerung == 0.0 && Ausgang_PID_Winkel != 0.0)
		{
			if (Ausgang_PID_Winkel > 0.0)
			{
				Setpoint_bias -= 0.00015;
			}
			else
			{
				Setpoint_bias += 0.00015;
			}
		}*/
	}
	else
	{
		analogWrite(Pin_Platinenluefter,0);
	}


	if (Umkippschutz(40,GET_KalmanWinkelY())==true)
	{
		MotorenEINAUS=false;
		Motoren_EINAUS_Schalten(MotorenEINAUS);
	}

	//PID_Regler_Geschwindigkeit.Compute();
	Akkuueberwachung(6,7);
	//Lueftersteuerung_Temperatur(GET_MPU_Temperatur(),Pin_Gehaeuseluefter);//Geh�usel�ftersteuerung
	//cmdMessenger.sendCmd(cmd_Anzeige_Text,analogRead(Akkuspannung2));

	cmdMessenger.feedinSerialData();//cmdMessenger Datenauslesen und Callbacks ausl�sen
}


long ISR_test = 0;



//Interrupt Routine zur Stepansteuerung der Motoren
ISR(TIMER2_COMPA_vect)
{
	bool Step_Rechts = Motor_Rechts.Step(); 
	bool Step_Links = Motor_Links.Step();

		if (Ausgang_PID_Winkel > 0.0)
		{
			digitalWrite(7, HIGH);
			digitalWrite(8, LOW);
		}
		else
		{
			digitalWrite(8, HIGH);
			digitalWrite(7, LOW);
		}		

	Ausgangsregister_schreiben(Step_Links, Step_Rechts);
		
	

	
}
