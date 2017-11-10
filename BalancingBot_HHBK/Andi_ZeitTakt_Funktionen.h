/*
 Name:		Andi_Stepper_Motor.h
 Created:	15.10.2017 15:38:15
 Author:	Andreas Bauer
*/
#ifndef _ANDI_ZeitTakt_FUNKTIONEN_h
	#define _ANDI_ZeitTakt_FUNKTIONEN_h

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "arduino.h"
	#else
		#include "WProgram.h"
	#endif

	class ZeitTakt_Funktionen
	{
	public:
		void ZeitTakt();//Takt der im loop durchgeführt werden muss
		bool ZeitTakt_20ms();//Abfage ob seit dem letzten Takt 20ms abgelaufen sind
		bool ZeitTakt_100ms();//Abfage ob seit dem letzten Takt 100ms abgelaufen sind
		bool ZeitTakt_1s();//Abfage ob seit dem letzten Takt 1s abgelaufen sind
		bool ZeitTakt_10s();//Abfage ob seit dem letzten Takt 10s abgelaufen sind

	private:
		unsigned long Startzeitpunkt=0;
		byte Anzahl_20ms_Takte=0;
		bool Taktmerker=false;//Merker der Zyklisch beschrieben wird wenn seit dem letzten wechsel 20ms abgelaufen sind
	};

	void ZeitTakt_Funktionen::ZeitTakt()
	{
		if (Anzahl_20ms_Takte>500)
		{
			Anzahl_20ms_Takte=1;
		}
		if (micros()>=(Startzeitpunkt+20000))
		{
			Startzeitpunkt=micros();
			Anzahl_20ms_Takte++;
			Taktmerker=true;
		}
		else
		{
			Taktmerker=false;
		}
	}
	bool ZeitTakt_Funktionen::ZeitTakt_20ms()
	{	
		return Taktmerker;
	}
	bool ZeitTakt_Funktionen::ZeitTakt_100ms()
	{
		if (Anzahl_20ms_Takte%5==0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool ZeitTakt_Funktionen::ZeitTakt_1s()
	{
	if (Anzahl_20ms_Takte%50==0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool ZeitTakt_Funktionen::ZeitTakt_10s()
	{
	if (Anzahl_20ms_Takte%500==0)
		{
			Anzahl_20ms_Takte=0;
			return true;
		}
		else
		{
			return false;
		}
	}




#endif