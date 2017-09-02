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
//Pin Setup Routine um alle Pins in den Richtigen Pin Mode zuversetzen und Passendere Namen zugeben 
void Pin_Setup();

//Spannungsteilerberechnung für Spannungskontrolle
double Spannungsteiler (double R1, double R2, int AnalogEingangsPin);
bool Akkuueberwachung (int AkkuPin1, int AkkuPin2);
	
#endif

