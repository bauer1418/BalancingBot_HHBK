/*
 Name:		BalancingBot_HHBK.ino
 Created:	19.07.2017 02:36:37
 Author:	Andreas Bauer
*/


unsigned long Zykluszeit=0;					//akutelle Zykluszeit
bool volatile MotorenEINAUS = false;		//Motoren Ein Aus Schalter 
volatile double Sollwertanpassung = 0.0;	//Anpassungswert für Stillstand

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
	PID_Regler_Winkel.SetSampleTime(50);
	PID_Regler_Winkel.SetOutputLimits(-400,400);//!!!!!!!!!!!!!!!!!!!!!!!!!!HIER IST DER FEHLER MIT PID AUSGANG NUR 0-255!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
		analogWrite(Pin_Platinenluefter,204);//80% Lüfterdrehzahl
		Eingang_PID_Geschwindigkeit=Ausgang_PID_Winkel;
		Sollwert_PID_Winkel=Ausgang_PID_Geschwindigkeit;
		
		//Winkel auslesen
		Eingang_PID_Winkel=GET_KalmanWinkelY();
		


		//if (Eingang_PID_Winkel>2+Sollwert_PID_Winkel || Eingang_PID_Winkel< Sollwert_PID_Winkel-2)
		//{
		//	PID_Regler_Winkel.SetTunings(System_Einstellungen.PID_Regler_Winkel_Fern_P, System_Einstellungen.PID_Regler_Winkel_Fern_I, System_Einstellungen.PID_Regler_Winkel_Fern_D);
		////	Motor_Links.StepMode_setzen(Stepper_Motor::Halbschritt);
		////	Motor_Rechts.StepMode_setzen(Stepper_Motor::Halbschritt);
		//}
		//else
		//{
		//PID_Regler_Winkel.SetTunings(System_Einstellungen.PID_Regler_Winkel_Nah_P, System_Einstellungen.PID_Regler_Winkel_Nah_I, System_Einstellungen.PID_Regler_Winkel_Nah_D);
		//	Motor_Links.StepMode_setzen(Stepper_Motor::Viertelschritt);
		//	Motor_Rechts.StepMode_setzen(Stepper_Motor::Viertelschritt);
	/*	}*/

		//Sollwertanpassung für Stillstand
		//Sollwert_PID_Winkel = Sollwert_Steuerung + Sollwertanpassung;
		
		//PID-Regler ausführen
		//PID_Regler_Geschwindigkeit.Compute();
		PID_Regler_Winkel.Compute();//PID-Regler für die Winkelsteuerung zyklisch ausführen
		
		  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Motor pulse calculations
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //To compensate for the non-linear behaviour of the stepper motors the folowing calculations are needed to get a linear speed behaviour.
  if(pid_output_left > 0)pid_output_left = 405 - (1/(pid_output_left + 9)) * 5500;
  else if(pid_output_left < 0)pid_output_left = -405 - (1/(pid_output_left - 9)) * 5500;

  if(pid_output_right > 0)pid_output_right = 405 - (1/(pid_output_right + 9)) * 5500;
  else if(pid_output_right < 0)pid_output_right = -405 - (1/(pid_output_right - 9)) * 5500;

  //Calculate the needed pulse time for the left and right stepper motor controllers
  if(pid_output_left > 0)left_motor = 400 - pid_output_left;
  else if(pid_output_left < 0)left_motor = -400 - pid_output_left;
  else left_motor = 0;

  if(pid_output_right > 0)right_motor = 400 - pid_output_right;
  else if(pid_output_right < 0)right_motor = -400 - pid_output_right;
  else right_motor = 0;

  //Copy the pulse time to the throttle variables so the interrupt subroutine can use them
  throttle_left_motor = left_motor;
  throttle_right_motor = right_motor;


		Motor_Rechts.Drehzahl_festlegen(Ausgang_PID_Winkel, 100);
		Motor_Links.Drehzahl_festlegen(Ausgang_PID_Winkel, 100);
	
	}
	else
	{
		//Sollwertanpassung=0.0;
		analogWrite(Pin_Platinenluefter,0);
	}


	if (Umkippschutz(25,GET_KalmanWinkelY())==true || Status==6)
	{
		MotorenEINAUS=false;
		Motoren_EINAUS_Schalten(MotorenEINAUS);
	}

	//PID_Regler_Geschwindigkeit.Compute();
	Akkuueberwachung(6,7);
	cmdMessenger.feedinSerialData();//cmdMessenger Datenauslesen und Callbacks auslösen
}


long ISR_test = 0;



//Interrupt Routine zur Stepansteuerung der Motoren
ISR(TIMER2_COMPA_vect)
{
		if (Ausgang_PID_Winkel > 0.0)
		{
			digitalWrite(7, LOW);
			digitalWrite(8, HIGH);
		}
		else
		{
			digitalWrite(8, LOW);
			digitalWrite(7, HIGH);
		}

		Ausgangsregister_schreiben(Motor_Links.Step(), Motor_Rechts.Step());
					
}
