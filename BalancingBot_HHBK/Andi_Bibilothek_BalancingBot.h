/*
 Name:		BalancingBot_HHBK.ino
 Created:	23.07.2017 14:32:37
 Author:	Andreas Bauer
*/
// Andi_Bibilothek_BalancingBot.h

#ifndef _ANDI_BIBILOTHEK_BALANCINGBOT_h
	#define _ANDI_BIBILOTHEK_BALANCINGBOT_h
	#if defined(ARDUINO) && ARDUINO >= 100
		#include "arduino.h"
	#else
		#include "WProgram.h"
	#endif

#ifndef _Wire_h
	#include <Wire.h>
#endif // !Wire.h

#ifndef _Kalman_h
	#include "Kalman.h"
#endif // !Kalman.h


//#include <Wire.h>
//#include "Kalman.h" // Source: https://github.com/TKJElectronics/KalmanFilter

/* Copyright (C) 2012 Kristian Lauszus, TKJ Electronics. All rights reserved.

 This software may be distributed and modified under the terms of the GNU
 General Public License version 2 (GPL2) as published by the Free Software
 Foundation and appearing in the file GPL2.TXT included in the packaging of
 this file. Please note that GPL2 Section 2[b] requires that all works based
 on this software must also be made publicly available under the terms of
 the GPL2 ("Copyleft").

 Contact information
 -------------------

 Kristian Lauszus, TKJ Electronics
 Web      :  http://www.tkjelectronics.com
 e-mail   :  kristianl@tkjelectronics.com
 */

#define RESTRICT_PITCH // Comment out to restrict roll to ±90deg instead - please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf
//double kalAngleX, kalAngleY; // Calculated angle using a Kalman filter



//Funktionsprotoypen

bool MPUsetup();
void MPU_Zyklus();
int GET_MPU_Status();

uint8_t i2cRead(uint8_t registerAddress, uint8_t *data, uint8_t nbytes);
uint8_t i2cWrite(uint8_t registerAddress, uint8_t *data, uint8_t length, bool sendStop);
uint8_t i2cWrite(uint8_t registerAddress, uint8_t data, bool sendStop);



//Variablen

	//Pinnummern Übersicht
	const int Pin_Stepmode_MS1 = 2;			//DigitalOutput Stepmode Umschaltung 1
	const int Pin_NeopixelData = 3;			//DigitalOutput Neopixel Daten Ausgang
	const int Pin_Stepmode_MS2 = 4;			//DigitalOutput Stepmode Umschaltung 1
	const int Pin_Platinenluefter = 5;		//PWMOutput Platinenlüfter Drehzahl für Transistor Q2
	const int Pin_Gehaeuseluefter = 6;		//PWMOutput Platinenlüfter Drehzahl für Transistor Q3
	const int Pin_DIR_Rechts = 7;			//DigitalOutput Richtungsauswahl Motor Rechts
	const int Pin_DIR_Links = 8;			//DigitalOutput Richtungsauswahl Motor Links
	const int Pin_Step_Rechts = 9;			//PWMOutput Step Befehl für Motor Rechts 
	const int Pin_Step_Links = 10;			//PWMOutput Step Befehl für Motor Links
	const int Pin_Sleep_Motortreiber = 11;	//DigitalOutput Motortreiber in den Schlafmodus versetzen 0=Schlafmodus 1=Betrieb
	const int Pin_Reset_Motortreiber = 12;	//DigitalOutput Fehler des Mototreibers zurücksetzen 1=Aktiv
	const int Pin_ENABLE_Motortreiber = 13;	//DigitalOutput Motortreiber Aktivieren 0=Aktiv 1=Inaktiv
	const int Pin_FAULT_Links = 14;			//DigitalInput Fehlereingang Motortreiber Links
	const int Pin_FAULT_Rechts =15;			//DigitalInput Fehlereingang Motortreiber Rechts
	const int Pin_Reserve_A2 = 16;			//Analog Reserve Pin
	const int Pin_Reserve_A3 = 17;			//Analog Reserve Pin
	const int Pin_Akku1_Messung = 18;		//AnalogInput Spannungsmessung Akku 1
	const int Pin_Akku2_Messung = 19;		//AnalogInput Spannungsmessung Akku 2

	

//Pin Setup Routine um alle Pins in den Richtigen Pin Mode zuversetzen und Passendere Namen zugeben 
void Pin_Setup();

//Spannungsteilerberechnung für Spannungskontrolle
double Spannungsteiler (double R1, double R2, int AnalogEingangsPin);
bool Akkuueberwachung (int AkkuPin1, int AkkuPin2);
double Akku_Messbereich_Berechnen(double AkkuMin, double AkkuMax);

double GET_KalmanWinkelX();//Gibt den aktuellen Kalmanwinkel auf der X Achse des MPUs aus
double GET_KalmanWinkelY();//Gibt den aktuellen Kalmanwinkel auf der Y Achse des MPUs aus

#endif

