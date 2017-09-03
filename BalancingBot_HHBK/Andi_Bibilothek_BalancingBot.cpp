/*
 Name:		BalancingBot_HHBK.ino
 Created:	23.07.2017 14:32:37
 Author:	Andreas Bauer
*/


#include "Andi_Bibilothek_BalancingBot.h"
#include "Messenger_Enum.h"
extern Kalman kalmanX; // Create the Kalman instances
extern Kalman kalmanY;// Create the Kalman instances

/* IMU Data */
double accX, accY, accZ;
double gyroX, gyroY, gyroZ;
int16_t tempRaw;

double gyroXangle, gyroYangle; // Angle calculate using the gyro only
double compAngleX, compAngleY; // Calculated angle using a complementary filter
double kalAngleX, kalAngleY; // Calculated angle using a Kalman filter
double offset_acelX, offset_acelY, offset_acelZ, offset_gyroX, offset_gyroY, offset_gyroZ;//Offsetwerte für MPU6050

uint32_t timer;
uint8_t i2cData[14]; // Buffer for I2C data

int MPU_Status=0;

//Pin Setup Routine um alle Pins in den Richtigen Pin Mode zuversetzen und Passendere Namen zugeben 
void Pin_Setup()
{
	
	//PinMode für alle Pins setzen
	pinMode(Pin_Stepmode_MS1,OUTPUT);
	pinMode(Pin_NeopixelData,OUTPUT);
	pinMode(Pin_Stepmode_MS2,OUTPUT);
	pinMode(Pin_Platinenluefter,OUTPUT);
	pinMode(Pin_Gehaeuseluefter,OUTPUT);
	pinMode(Pin_DIR_Rechts,OUTPUT);
	pinMode(Pin_DIR_Links,OUTPUT);
	pinMode(Pin_Step_Rechts,OUTPUT);
	pinMode(Pin_Step_Links,OUTPUT);
	pinMode(Pin_Sleep_Motortreiber,OUTPUT);
	pinMode(Pin_Reset_Motortreiber,OUTPUT);
	pinMode(Pin_ENABLE_Motortreiber,OUTPUT);
	pinMode(Pin_FAULT_Links,INPUT);
	pinMode(Pin_FAULT_Rechts,INPUT);
	pinMode(Pin_Akku1_Messung,INPUT);
	pinMode(Pin_Akku2_Messung,INPUT);

}

//Spannungsteilerberechnung für Spannungskontrolle
double Spannungsteiler (double R1, double R2, int AnalogEingangsPin)
	
{
	double Strom=0.00;
	double Spannung=-70.00;
	pinMode(AnalogEingangsPin, INPUT);
	Strom=(analogRead(AnalogEingangsPin)*5.00/1024.00)/R1;
	Spannung=(R1+R2)*Strom;
	return Spannung;
}



//Spannungsbereich für die Akkumessungen berechnen Beispiel 8,4V-6V=2,4V
double Akku_Messbereich_Berechnen(double AkkuMin, double AkkuMax)
{
	double Messbereich =0.00;
	Messbereich= AkkuMax-AkkuMin;
	return Messbereich;
}

//Gibt den aktuellen Kalmanwinkel auf der X Achse des MPUs aus
double GET_KalmanWinkelX()
{
	return kalAngleX;
}
//Gibt den aktuellen Kalmanwinkel auf der Y Achse des MPUs aus
double GET_KalmanWinkelY()
{
	return kalAngleY;
}

int GET_MPU_Status()
{
	return MPU_Status;
}

//MPU Setup mit I2C-Start Bei Fehlern wird ein Fehlercode über den ComPort geschickt
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
	MPU_Status=MPU_NOT_FOUND;
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
  gyroXangle = roll;
  gyroYangle = pitch;
  compAngleX = roll;
  compAngleY = pitch;

  timer = micros();
  MPU_Status=Kein_Fehler_vorhanden;
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
    compAngleX = roll;
    kalAngleX = roll;
    gyroXangle = roll;
  } else
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleX) > 90)
    gyroYrate = -gyroYrate; // Invert rate, so it fits the restriced accelerometer reading
  //kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
else
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90)) {
    kalmanY.setAngle(pitch);
    compAngleY = pitch;
    kalAngleY = pitch;
    gyroYangle = pitch;
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

#if 0 // Set to 1 to print the temperature
  Serial.print("\t");

  double temperature = (double)tempRaw / 340.0 + 36.53;
  Serial.print(temperature); Serial.print("\t");
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
    Serial.print(F("i2cWrite failed: "));
    Serial.println(rcode);
  }
  return rcode; // See: http://arduino.cc/en/Reference/WireEndTransmission
}

uint8_t i2cRead(uint8_t registerAddress, uint8_t *data, uint8_t nbytes) {
  uint32_t timeOutTimer;
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  uint8_t rcode = Wire.endTransmission(false); // Don't release the bus
  if (rcode) {
    Serial.print(F("i2cRead failed: "));
    Serial.println(rcode);
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
        Serial.println(F("i2cRead timeout"));
        return 5; // This error value is not already taken by endTransmission
      }
    }
  }
  return 0; // Success
}
