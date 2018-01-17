#ifndef _MPU6050_BALANCINGBOT_HHBK_h
	#define	_MPU6050_BALANCINGBOT_HHBK_h


	#if defined(ARDUINO) && ARDUINO >= 100
		#include "arduino.h"
	#else
		#include "WProgram.h"
	#endif

	//Include Anweisungen
	#include <CmdMessenger.h>
	#include "Messenger_Enum.h"
	#include "Messenger_Befehle.h"


	#include <Wire.h>
	#include "Kalman.h" // Source: https://github.com/TKJElectronics/KalmanFilter

#define RESTRICT_PITCH // Comment out to restrict roll to ±90deg instead - please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf

Kalman kalmanX; // Create the Kalman instances
Kalman kalmanY;// Create the Kalman instances

/* IMU Data */
double accX, accY, accZ;
double gyroX, gyroY, gyroZ;
int16_t tempRaw;

double temperature = 0.0;//Temperatur des MPU6050
//double gyroXangle, gyroYangle; // Angle calculate using the gyro only
//double compAngleX, compAngleY; // Calculated angle using a complementary filter
double  kalAngleX, kalAngleY; // Calculated angle using a Kalman filter
double offset_acelX, offset_acelY, offset_acelZ, offset_gyroX, offset_gyroY, offset_gyroZ;//Offsetwerte für MPU6050

uint32_t timer;
uint8_t i2cData[14]; // Buffer for I2C data

uint8_t i2cRead(uint8_t registerAddress, uint8_t *data, uint8_t nbytes);
uint8_t i2cWrite(uint8_t registerAddress, uint8_t *data, uint8_t length, bool sendStop);
uint8_t i2cWrite(uint8_t registerAddress, uint8_t data, bool sendStop);




	//Funktionprotypen
	double GET_KalmanWinkelY();
	double GET_KalmanWinkelX();
	double GET_MPU_Temperatur();
	double Offset_acelx();
	void Offset_acelx(double OffsetWert);
	double Offset_acely();
	void Offset_acely(double OffsetWert);
	double Offset_acelz();
	void Offset_acelz(double OffsetWert);
	double Offset_gyrox();
	void Offset_gyrox(double OffsetWert);
	double Offset_gyroy();
	void Offset_gyroy(double OffsetWert);
	double Offset_gyroz();
	void Offset_gyroz(double OffsetWert);
	void MPU6050_Kalibrieren();
	bool MPUsetup();
	void MPU_Zyklus();



#endif // !_MPU6050_BALANCINGBOT_HHBK_h_