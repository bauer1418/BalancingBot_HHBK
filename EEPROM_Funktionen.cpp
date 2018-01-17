// 
// 
// 

#include "EEPROM_Funktionen.h"
#include "Andi_Bibilothek_BalancingBot.h"
#include"TEST.h"

//Daten aus dem EEPROM auslesen von Adresse 0 startend
//return Einstellungen
Balancing_Bot_Einstellungen Daten_aus_EEPROM_lesen()
{
	Balancing_Bot_Einstellungen EEPROM_Daten;
	EEPROM.get(0,EEPROM_Daten);
	return EEPROM_Daten;
}



void Einstellungen_mit_Standart_Werten_beschreiben()
{
	System_Einstellungen.Werte_ueberschrieben=0;					//Standartwert wie oft das EEPROM überschrieben wurde
	System_Einstellungen.PID_Regler_Winkel_P=0;						//P-Einstellung für Winkel-Regler
	System_Einstellungen.PID_Regler_Winkel_I=0;						//I-Einstellung für Winkel-Regler
	System_Einstellungen.PID_Regler_Winkel_D=0;						//D-Einstellung für Winkel-Regler
	System_Einstellungen.PID_Regler_Geschwindigkeit_P=0;			//P-Einstellung für Geschwindigkeits-Regler
	System_Einstellungen.PID_Regler_Geschwindigkeit_I=0;			//I-Einstellung für Geschwindigkeits-Regler
	System_Einstellungen.PID_Regler_Geschwindigkeit_D=0;			//D-Einstellung für Geschwindigkeits-Regler
	System_Einstellungen.PID_Regler_Winkel_Min=-120;				//Minimaler Ausgangswert des Winkel-Reglers
	System_Einstellungen.PID_Regler_Winkel_Max=120;					//Maximaler Ausgangswert des Winkel-Reglers
	System_Einstellungen.PID_Regler_Geschwindigkeit_Min=-5;			//Minimaler Ausgangswert des Geschwindigkeits-Reglers
	System_Einstellungen.PID_Regler_Geschwindigkeit_Max=5;			//Maximaler Ausgangswert des Geschwindigkeits-Reglers
	System_Einstellungen.Schrittmodus=0;	//Stepmodus der Motorregler
	System_Einstellungen.Platinenluefter_Sollwert=207;				//Sollwert für Platinenlüfter PWM
	System_Einstellungen.Gehaeuseluefter_Sollwert=207;				//Sollwert für Gehäuselüfter PWM
	System_Einstellungen.NeoPixelRing_Helligkeit=25;				//Helligkeit NeoPixelRing
	System_Einstellungen.MPU_Offset_AcelX=0;						//Offsetwert für MPU AcelX
	System_Einstellungen.MPU_Offset_AcelY=0;						//Offsetwert für MPU AcelY
	System_Einstellungen.MPU_Offset_AcelZ=0;						//Offsetwert für MPU AcelZ
	System_Einstellungen.MPU_Offset_GyroX=0;						//Offsetwert für MPU GyroX
	System_Einstellungen.MPU_Offset_GyroY=0;						//Offsetwert für MPU GyroY
	System_Einstellungen.MPU_Offset_GyroZ=0;						//Offsetwert für MPU GyroZ


}
void EEPROM_Werte_aktiveren(Balancing_Bot_Einstellungen EEPROM_Daten)
{
	Offset_acelx(EEPROM_Daten.MPU_Offset_AcelX);
	Offset_acely(EEPROM_Daten.MPU_Offset_AcelY);
	Offset_acelz(EEPROM_Daten.MPU_Offset_AcelZ);
	Offset_gyrox(EEPROM_Daten.MPU_Offset_GyroX);
	Offset_gyroy(EEPROM_Daten.MPU_Offset_GyroY);
	Offset_gyroz(EEPROM_Daten.MPU_Offset_GyroZ);

	PID_Regler_Winkel.SetTunings(EEPROM_Daten.PID_Regler_Winkel_P,EEPROM_Daten.PID_Regler_Winkel_I,EEPROM_Daten.PID_Regler_Winkel_D);
	PID_Regler_Winkel.SetOutputLimits(EEPROM_Daten.PID_Regler_Winkel_Min,EEPROM_Daten.PID_Regler_Winkel_Max);
	PID_Regler_Geschwindigkeit.SetTunings(EEPROM_Daten.PID_Regler_Geschwindigkeit_P,EEPROM_Daten.PID_Regler_Geschwindigkeit_I,EEPROM_Daten.PID_Regler_Geschwindigkeit_D);
	PID_Regler_Geschwindigkeit.SetOutputLimits(EEPROM_Daten.PID_Regler_Geschwindigkeit_Min,EEPROM_Daten.PID_Regler_Geschwindigkeit_Max);

	Motor_Links.StepMode_setzen(EEPROM_Daten.Schrittmodus);
	Motor_Rechts.StepMode_setzen(EEPROM_Daten.Schrittmodus);

	//NeoPixel_Setup(EEPROM_Daten.NeoPixelRing_Helligkeit);

	analogWrite(Pin_Platinenluefter,EEPROM_Daten.Platinenluefter_Sollwert);
	analogWrite(Pin_Gehaeuseluefter,EEPROM_Daten.Gehaeuseluefter_Sollwert);

}