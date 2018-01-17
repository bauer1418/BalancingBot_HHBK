/*
 Name:		BalancingBot_HHBK.ino
 Created:	19.07.2017 02:36:37
 Author:	Andreas Bauer
*/


unsigned long Zykluszeit=0;					//akutelle Zykluszeit
bool volatile MotorenEINAUS = false;		//Motoren Ein Aus Schalter 
volatile double Sollwertanpassung = 0.0;	//Anpassungswert für Stillstand

//Benötigte Bibilotheken
#include <Adafruit_NeoPixel.h>
#include "Andi_Bibilothek_BalancingBot.h"
#include "MPU6050_BalancingBot_HHBK.h"
#include "Messenger_Befehle.h"
#include "Andi_ZeitTakt_Funktionen.h"

double PIDOUT_Faktor = 1.0;//Faktor in Linear Funktion unter Bruchstrich Orginal 9.0
double Linear_Multiplikationsfaktor = 5000.0;//Wert mit dem die Linearfuktion multipliziert wird Orginalwert = 5500
bool Akkustatus = true; //Akku i.O. = true n.i.O. = false

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Declaring global variables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte start, received_byte, low_bat;

int left_motor, throttle_left_motor, throttle_counter_left_motor, throttle_left_motor_memory;
int right_motor, throttle_right_motor, throttle_counter_right_motor, throttle_right_motor_memory;
int battery_voltage;
int receive_counter;
int gyro_pitch_data_raw, gyro_yaw_data_raw, accelerometer_data_raw;

long gyro_yaw_calibration_value, gyro_pitch_calibration_value;

unsigned long loop_timer;

float angle_gyro, angle_acc, angle, self_balance_pid_setpoint;
float pid_error_temp, pid_i_mem, pid_setpoint, gyro_input, pid_output, pid_last_d_error;
float pid_output_left, pid_output_right;


 //the setup function runs once when you press reset or power the board
void setup() 
{
	Interrupt_Setup();

	Pin_Setup();

	Setup_cmdMessenger();

	NeoPixel_Setup(25);
	MPUsetup();
	/*PID_Regler_Geschwindigkeit.SetControllerDirection(DIRECT);*/
	PID_Regler_Geschwindigkeit.SetMode(AUTOMATIC);
	PID_Regler_Geschwindigkeit.SetSampleTime(20);
	PID_Regler_Geschwindigkeit.SetOutputLimits(-10,10);
	Sollwert_PID_Geschwindigkeit=0.00;

	/*PID_Regler_Winkel.SetControllerDirection(DIRECT);*/
	PID_Regler_Winkel.SetMode(AUTOMATIC);
	PID_Regler_Winkel.SetSampleTime(20);
	PID_Regler_Winkel.SetOutputLimits(-200,200);//!!!!!!!!!!!!!!!!!!!!!!!!!!HIER IST DER FEHLER MIT PID AUSGANG NUR 0-255!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Sollwert_PID_Winkel=0.00;
	Motor_Links.StepMode_setzen(Stepper_Motor::Halbschritt);
	Motor_Rechts.StepMode_setzen(Stepper_Motor::Halbschritt);
	Statusmeldung();
}

 //the loop function runs over and over again until power down or reset
void loop() 
{
	Zykluszeit = Zykluszeit_Messung();
	Allgemeine_Zeitfunktion.ZeitTakt();
	
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
		PID_Regler_Winkel.SetMode(AUTOMATIC);
		analogWrite(Pin_Platinenluefter,204);//80% Lüfterdrehzahl
		
		//Eingang_PID_Geschwindigkeit=-Ausgang_PID_Winkel;
		//PID_Regler_Geschwindigkeit.Compute();
		//Sollwert_PID_Winkel=Ausgang_PID_Geschwindigkeit;
		
		//Winkel auslesen
		Eingang_PID_Winkel=GET_KalmanWinkelY();
		


		//Sollwertanpassung für Stillstand
		Sollwert_PID_Winkel = Sollwert_Steuerung + Sollwertanpassung;
		//Sollwert_PID_Winkel = 0.0;

		//PID-Regler ausführen
		PID_Regler_Winkel.Compute();//PID-Regler für die Winkelsteuerung zyklisch ausführen
		pid_output = Ausgang_PID_Winkel;
		//Serial.print("PIDraw ");
		//Serial.println(pid_output);
		if (Ausgang_PID_Winkel < 2.0 && Ausgang_PID_Winkel > -2.0)pid_output = 0.0;                      //Create a dead-band to stop the motors when the robot is balanced



		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Control calculations
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		pid_output_left = pid_output;                                             //Copy the controller output to the pid_output_left variable for the left motor
		pid_output_right = pid_output;                                            //Copy the controller output to the pid_output_right variable for the right motor
		//Serial.print("PID_Out: ");
		//Serial.println(pid_output_left);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Motor pulse calculations
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//To compensate for the non-linear behaviour of the stepper motors the folowing calculations are needed to get a linear speed behaviour.

		//if (pid_output_left == 0.0)pid_output_left = -5095.0;
		if (pid_output_left > 0.0)pid_output_left = 405.0 - (1.0 / (pid_output_left + PIDOUT_Faktor)) * Linear_Multiplikationsfaktor;
		else if (pid_output_left < 0.0)pid_output_left = -405.0 - (1.0 / (pid_output_left - PIDOUT_Faktor)) * Linear_Multiplikationsfaktor;
		//Serial.print("Lin-Left");
		//Serial.println(pid_output_left);
		//if (pid_output_right == 0.0)pid_output_right = -5095.0;
		if (pid_output_right > 0.0)pid_output_right = 405.0 - (1.0 / (pid_output_right + PIDOUT_Faktor)) * Linear_Multiplikationsfaktor;
		else if (pid_output_right < 0.0)pid_output_right = -405.0 - (1.0 / (pid_output_right - PIDOUT_Faktor)) * Linear_Multiplikationsfaktor;

		//Calculate the needed pulse time for the left and right stepper motor controllers
		if (pid_output > 0.0)left_motor = 400.0 - pid_output_left;
		else if (pid_output < 0.0)left_motor = -400.0 - pid_output_left;
		else left_motor = 0.0;

		if (pid_output > 0.0)right_motor = 400.0 - pid_output_right;
		else if (pid_output< 0.0)right_motor = -400.0 - pid_output_right;
		else right_motor = 0.0;

		//Copy the pulse time to the throttle variables so the interrupt subroutine can use them
		throttle_left_motor = left_motor;
		throttle_right_motor = right_motor;

		//The self balancing point is adjusted when there is not forward or backwards movement from the transmitter. This way the robot will always find it's balancing point
		if (Sollwert_Steuerung == 0.0) //If the setpoint is zero degrees
		{                                              
			if (Ausgang_PID_Winkel < 0.0)Sollwertanpassung -= 0.003;  //Increase the self_balance_pid_setpoint if the robot is still moving forewards
			if (Ausgang_PID_Winkel > 0.0)Sollwertanpassung += 0.003;  //Decrease the self_balance_pid_setpoint if the robot is still moving backwards
		}

		////Motorendrehzahl festlegen
		//Motor_Rechts.Drehzahl_festlegen(Ausgang_PID_Winkel, 100);
		//Motor_Links.Drehzahl_festlegen(Ausgang_PID_Winkel, 100);
	
	}
	else
	{
		Sollwertanpassung=0.0;
		analogWrite(Pin_Platinenluefter,0);
		PID_Regler_Winkel.Ruecksetzen_P_I_D_Teile();
	}


	if (Umkippschutz(25,GET_KalmanWinkelY())==true || Status==6)
	{
		MotorenEINAUS=false;
		Motoren_EINAUS_Schalten(MotorenEINAUS);
	}

	Akkuueberwachung(6,7);
	cmdMessenger.feedinSerialData();//cmdMessenger Daten auslesen und Callbacks auslösen
}

//Interrupt Routine zur Stepansteuerung der Motoren
ISR(TIMER2_COMPA_vect)
{		//Left motor pulse calculations
		throttle_counter_left_motor++;                                           //Increase the throttle_counter_left_motor variable by 1 every time this routine is executed
		if (throttle_counter_left_motor > throttle_left_motor_memory) {             //If the number of loops is larger then the throttle_left_motor_memory variable
			throttle_counter_left_motor = 0;                                        //Reset the throttle_counter_left_motor variable
			throttle_left_motor_memory = throttle_left_motor;                       //Load the next throttle_left_motor variable
			if (throttle_left_motor_memory < 0) {                                     //If the throttle_left_motor_memory is negative
																					  //PORTD &= 0b11110111;                                                  //Set output 3 low to reverse the direction of the stepper controller
																					  //PORTD=B11110111;
				digitalWrite(8, 1);
				throttle_left_motor_memory *= -1;                                     //Invert the throttle_left_motor_memory variable
			}
			else digitalWrite(8, 0)/*PORTD |= B00001000*/;												//Set output 3 high for a forward direction of the stepper motor
																										//PORTD |= 0b00001000;
		}
		else if (throttle_counter_left_motor == 1)digitalWrite(10, 1);//PORTD |= B00000100;             //Set output 2 high to create a pulse for the stepper controller
		else if (throttle_counter_left_motor == 2)digitalWrite(10, 0);//PORTD &= B11111011;             //Set output 2 low because the pulse only has to last for 20us 

																	  //right motor pulse calculations
		throttle_counter_right_motor++;                                          //Increase the throttle_counter_right_motor variable by 1 every time the routine is executed
		if (throttle_counter_right_motor > throttle_right_motor_memory) {           //If the number of loops is larger then the throttle_right_motor_memory variable
			throttle_counter_right_motor = 0;                                       //Reset the throttle_counter_right_motor variable
			throttle_right_motor_memory = throttle_right_motor;                     //Load the next throttle_right_motor variable
			if (throttle_right_motor_memory < 0) {                                    //If the throttle_right_motor_memory is negative
				digitalWrite(7, 0);
				//PORTD |= B00100000;                                                  //Set output 5 low to reverse the direction of the stepper controller
				throttle_right_motor_memory *= -1;                                    //Invert the throttle_right_motor_memory variable
			}
			else  digitalWrite(7, 1);//PORTD &= B11011111;                                               //Set output 5 high for a forward direction of the stepper motor
		}
		else if (throttle_counter_right_motor == 1)digitalWrite(9, 1);//PORTD |= B00010000;            //Set output 4 high to create a pulse for the stepper controller
		else if (throttle_counter_right_motor == 2)digitalWrite(9, 0);//PORTD &= B11101111;            //Set output 4 low because the pulse only has to last for 20us
	}
		////Richtung einstellen über PIN HIGH oder LOW direkt
		////Ist zwar bereits in dem Stepper Objekt vorhanden hat aber keinen Einfluss wenn die ISR benutzt wird
		////Herr Wisseman konnte dies auch nicht beantworten!
		//if (Ausgang_PID_Winkel > 0.0)
		//{
		//	digitalWrite(7, LOW);
		//	digitalWrite(8, HIGH);
		//}
		//else
		//{
		//	digitalWrite(8, LOW);
		//	digitalWrite(7, HIGH);
		//}

		//Ausgangsregister_schreiben(Motor_Links.Step(), Motor_Rechts.Step());