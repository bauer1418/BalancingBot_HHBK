#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

 #include <Wire.h>
#include "Kalman.h" // Source: https://github.com/TKJElectronics/KalmanFilter

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




//Aktueller Kalmanwinkel für die Y-Achse des MPU6050
double GET_KalmanWinkelY()
{
	return kalAngleY;
}
//Aktueller Kalmanwinkel für die X-Achse des MPU6050
double GET_KalmanWinkelX()
{
	return kalAngleX;
}
//Aktuelle Temperatur des MPU6050
double GET_MPU_Temperatur()
{
	return temperature;
}
//Auslesen der aktuellen Offset MPU Werte für acelx
double Offset_acelx()
{
	return offset_acelX;
}
//MPU Offset Wert für acelx setzen
void Offset_acelx(double OffsetWert)
{
	offset_acelX= OffsetWert;

}
//Auslesen der aktuellen Offset MPU Werte für acely
double Offset_acely()
{
	return offset_acelY;
}
//MPU Offset Wert für acely setzen
void Offset_acely(double OffsetWert)
{
	offset_acelY= OffsetWert;

}
//Auslesen der aktuellen MPU Offset Werte für acelz
double Offset_acelz()
{
	return offset_acelZ;
}
//MPU Offset Wert für acelz setzen
void Offset_acelz(double OffsetWert)
{
	offset_acelZ= OffsetWert;

}

//Auslesen der aktuellen Offset MPU Werte für gyrox
double Offset_gyrox()
{
	return offset_gyroX;
}
//MPU Offset Wert für gyrox setzen
void Offset_gyrox(double OffsetWert)
{
	offset_gyroX= OffsetWert;

}
//Auslesen der aktuellen Offset MPU Werte für gyroy
double Offset_gyroy()
{
	return offset_gyroY;
}
//MPU Offset Wert für gyroy setzen
void Offset_gyroy(double OffsetWert)
{
	offset_gyroY= OffsetWert;

}
//Auslesen der aktuellen MPU Offset Werte für gyroz
double Offset_gyroz()
{
	return offset_gyroZ;
}
//MPU Offset Wert für gyroz setzen
void Offset_gyroz(double OffsetWert)
{
	offset_gyroZ= OffsetWert;

}
//MPU6050 Automatik-Kalibrierung 
//Es werden für die 6 verschiedene Offsetwerte ein Durchschnittswert aus 100 Messwerten gebildet sollange der Sensor nicht bewegt wird.
void MPU6050_Kalibrieren()
{
			double Cal_accX, Cal_accY, Cal_accZ, Cal_gyoX, Cal_gyoY, Cal_gyoZ;
			//Kalibrierwerte auf Startwerte des MPU stellen
			Cal_accX=accX;
			Cal_accY=accY;
			Cal_accZ=accZ;
			Cal_gyoX=gyroX;
			Cal_gyoY=gyroY;
			Cal_gyoZ=gyroZ;

			cmdMessenger.sendCmd(cmd_Anzeige_Text,F("Kalibrierung aktiv"));
			for (int i = 1; i < 99; i++)
			{
				MPU_Zyklus();
				Cal_accX=(Cal_accX+accX)/2;
				Cal_accY=(Cal_accY+accY)/2;
				Cal_accZ=(Cal_accZ+accZ)/2;
				Cal_gyoX=(Cal_gyoX+gyroX)/2;
				Cal_gyoY=(Cal_gyoY+gyroY)/2;
				Cal_gyoZ=(Cal_gyoZ+gyroZ)/2;
				delay(100);
				cmdMessenger.sendCmd(cmd_Anzeige_Text,F("."));
			}
			offset_acelX=0-Cal_accX;
			offset_acelY=0-Cal_accY;
			offset_acelZ=16384-Cal_accZ;
			offset_gyroX=0-Cal_gyoX;
			offset_gyroY=0-Cal_gyoY;
			offset_gyroZ=0-Cal_gyoZ;
}



//MPU Setup mit I2C-Start
//Bei Fehlern wird der Fehlercode in den Systemstatus geschrieben und es wird FALSE zurückgeschickt
//Rückgabewerte TRUE Setup erfolgreich beendet FALSE Setup Routine hat einen Fehler gehabt
bool MPUsetup() {
//Serial.begin(115200);
  Wire.begin();
  TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz

  i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
  i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
  i2cData[2] = 0x00; // Set Gyro Full Scale Range to ±250deg/s
  i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to ±2g
  while (i2cWrite(0x19, i2cData, 4, false)); // Write to all four registers at once
  while (i2cWrite(0x1A, 0x06, false));//DLPF Lowpassfilter Motorvibrationen siehe: http://meineweltinmeinemkopf.blogspot.de/2013/05/dlpf-einstellen-fur-den-mpu6050.html 
  while (i2cWrite(0x6B, 0x01, true)); // PLL with X axis gyroscope reference and disable sleep mode
  
  while (i2cRead(0x75, i2cData, 1));
  if (i2cData[0] != 0x68) { // Read "WHO_AM_I" register
		
	  //Serial.print(F("Error reading sensor"));
	Fehlerspeicher=8;
	Wire.end();
	return false;
		//while (1);
	
  }

  delay(100); // Wait for sensor to stabilize

  /* Set kalman and gyro starting angle */
  while (i2cRead(0x3B, i2cData, 6));
  accX = (i2cData[0] << 8) | i2cData[1];
  accY = (i2cData[2] << 8) | i2cData[3];
  accZ = (i2cData[4] << 8) | i2cData[5];

  accX=accX+offset_acelX;
  accY=accY+offset_acelY;
  accZ=accZ+offset_acelZ;
  gyroX=gyroX+offset_gyroX;
  gyroY=gyroY+offset_gyroY;
  gyroZ=gyroZ+offset_gyroZ;

  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2 outputs the value of -? to ? (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
#ifdef RESTRICT_PITCH // Eq. 25 and 26
  double roll  = atan2(accY, accZ) * RAD_TO_DEG;
  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else // Eq. 28 and 29
  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif

  kalmanX.setAngle(roll); // Set starting angle
  kalmanY.setAngle(pitch);
  //gyroXangle = roll;
  //gyroYangle = pitch;
  //compAngleX = roll;
  //compAngleY = pitch;

  timer = micros();
  Fehlerspeicher=MPU_NOT_FOUND;
  Status=MPU_Setup_OK;

  return true;

}

//MPU Hauptprogramm MPU6050 Zyklisch auslesen und Kalmanfilter anwenden
void MPU_Zyklus()
{
   
  /* Update all the values */
  while (i2cRead(0x3B, i2cData, 14));
  accX = ((i2cData[0] << 8) | i2cData[1]);
  accY = ((i2cData[2] << 8) | i2cData[3]);
  accZ = ((i2cData[4] << 8) | i2cData[5]);
  tempRaw = (i2cData[6] << 8) | i2cData[7];
  gyroX = (i2cData[8] << 8) | i2cData[9];
  gyroY = (i2cData[10] << 8) | i2cData[11];
  gyroZ = (i2cData[12] << 8) | i2cData[13];

  accX=accX+offset_acelX;
  accY=accY+offset_acelY;
  accZ=accZ+offset_acelZ;
  gyroX=gyroX+offset_gyroX;
  gyroY=gyroY+offset_gyroY;
  gyroZ=gyroZ+offset_gyroZ;



  double dt = (double)(micros() - timer) / 1000000; // Calculate delta time
  timer = micros();

  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2 outputs the value of -? to ? (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
#ifdef RESTRICT_PITCH // Eq. 25 and 26
  double roll  = atan2(accY, accZ) * RAD_TO_DEG;
  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else // Eq. 28 and 29
  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif

  double gyroXrate = gyroX / 131.0; // Convert to deg/s
  double gyroYrate = gyroY / 131.0; // Convert to deg/s

#ifdef RESTRICT_PITCH
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) {
    kalmanX.setAngle(roll);
    //compAngleX = roll;
    kalAngleX = roll;
    //gyroXangle = roll;
  } else
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleX) > 90)
    gyroYrate = -gyroYrate; // Invert rate, so it fits the restriced accelerometer reading
  //kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
else
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90)) {
    kalmanY.setAngle(pitch);
    //compAngleY = pitch;
    kalAngleY = pitch;
    //gyroYangle = pitch;
  } else
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleY) > 90)
    gyroXrate = -gyroXrate; // Invert rate, so it fits the restriced accelerometer reading
  kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter
#endif

  //gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter
  //gyroYangle += gyroYrate * dt;
  //gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate
  //gyroYangle += kalmanY.getRate() * dt;

  //compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll; // Calculate the angle using a Complimentary filter
  //compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;

  //// Reset the gyro angle when it has drifted too much
  //if (gyroXangle < -180 || gyroXangle > 180)
  //  gyroXangle = kalAngleX;
  //if (gyroYangle < -180 || gyroYangle > 180)
  //  gyroYangle = kalAngleY;

  /* Print Data */
#if 0 // Set to 1 to activate
  Serial.print(accX); Serial.print("\t");
  //Serial.print(accY); Serial.print("\t");
  //Serial.print(accZ); Serial.print("\t");

  Serial.print(gyroX); Serial.print("\t");
  //Serial.print(gyroY); Serial.print("\t");
  //Serial.print(gyroZ); Serial.print("\t");

  Serial.print("\t");
#endif

  /*Serial.print(roll); Serial.print("\t");*/
  //Serial.print(gyroXangle); Serial.print("\t");
  //Serial.print(compAngleX); Serial.print("\t");
  /*Serial.print(kalAngleX); Serial.print("\t");*/

 /* Serial.print("\t");*/

  //Serial.print(pitch); Serial.print("\t");
  //Serial.print(gyroYangle); Serial.print("\t");
  //Serial.print(compAngleY); Serial.print("\t");
  //Serial.print(kalAngleY); Serial.print("\t");

#if 1 // Set to 1 to print the temperature
  //Serial.print("\t");

  temperature = (double)tempRaw / 340.0 + 36.53;
  //Serial.print(temperature); Serial.print("\t");
#endif

}
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

const uint8_t IMUAddress = 0x68; // AD0 is logic low on the PCB
const uint16_t I2C_TIMEOUT = 1000; // Used to check for errors in I2C communication

uint8_t i2cWrite(uint8_t registerAddress, uint8_t data, bool sendStop) {
  return i2cWrite(registerAddress, &data, 1, sendStop); // Returns 0 on success
}

uint8_t i2cWrite(uint8_t registerAddress, uint8_t *data, uint8_t length, bool sendStop) {
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  Wire.write(data, length);
  uint8_t rcode = Wire.endTransmission(sendStop); // Returns 0 on success
  if (rcode) {
    
	  Status=Fehler;
	Fehlerspeicher=MPU_Write_FAILED;
	  /*Serial.print(F("i2cWrite failed: "));
    Serial.println(rcode);*/
  }
  return rcode; // See: http://arduino.cc/en/Reference/WireEndTransmission
}

uint8_t i2cRead(uint8_t registerAddress, uint8_t *data, uint8_t nbytes) {
  uint32_t timeOutTimer;
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  uint8_t rcode = Wire.endTransmission(false); // Don't release the bus
  if (rcode) {
	Status=Fehler;
	Fehlerspeicher=MPU_READ_FAILED;
    /*Serial.print(F("i2cRead failed: "));
    Serial.println(rcode);*/
    return rcode; // See: http://arduino.cc/en/Reference/WireEndTransmission
  }
  Wire.requestFrom(IMUAddress, nbytes, (uint8_t)true); // Send a repeated start and then release the bus after reading
  for (uint8_t i = 0; i < nbytes; i++) {
    if (Wire.available())
      data[i] = Wire.read();
    else {
      timeOutTimer = micros();
      while (((micros() - timeOutTimer) < I2C_TIMEOUT) && !Wire.available());
      if (Wire.available())
        data[i] = Wire.read();
      else {
		Status=Fehler;
		Fehlerspeicher=MPU_READ_TIMEOUT;
        /*Serial.println(F("i2cRead timeout"));*/
        return 5; // This error value is not already taken by endTransmission
      }
    }
  }
  return 0; // Success
}
