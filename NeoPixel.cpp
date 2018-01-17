
	//Include Anweisungen
	#include "Andi_Bibilothek_BalancingBot.h"
	#include "Messenger_Enum.h"
	#include <Adafruit_NeoPixel.h>
	#include "NeoPixel.h"

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, Pin_NeopixelData, NEO_GRB + NEO_KHZ800);
long letzter_NeoPixel_Wechsel=0;
unsigned int aktueller_Pixel=0;
bool Fehler_LED_AN=false;

  //// Some example procedures showing how to display to the pixels:
  //colorWipe(strip.Color(0,0,0), 25); // Black
  //colorWipe(strip.Color(64, 0, 0), 100); // Red
  //colorWipe(strip.Color(0, 64, 0), 100); // Green
  //colorWipe(strip.Color(0, 0, 64), 100); // Blue
  //colorWave(75);
  //colorWipe(strip.Color(0,0,0), 100); // Black
  //rainbow(15);
  //colorWipe(strip.Color(0,0,0), 100); // Black
  //rainbowCycle(15);
  //colorWipe(strip.Color(0,0,0), 100); // Black
 //colorWave(30);




//NeoPixel Setup Routine der Parameter Helligkeit steht für die Helligkeit des Pixel Rings im Bereich von 0-255 
void NeoPixel_Setup(byte Helligkeit)
{
	strip.begin();
	strip.setBrightness(Helligkeit);	 //Achtung 100% Helligkeit = Augentod
	strip.show();						// Alle Pixel auf aus stellen
}

//Haupt Neopixel Steuerung
//Wenn System gestört ist blinken oder im normal Fahrbetrieb Akkustand anzeigen
void NeoPixel_Steuerung(int Systemstatus, bool Motorenstatus)
{
	if (Systemstatus==Fehler)
	{
		
	}
	else if (Motorenstatus==true)
	{

	}
	else
	{

	}
}

//Akkustand anzeigen im Fahrbetrieb
void NeoPixel_Akkustand(int AkkuProzent)
{
	NeoPixel_alle_Pixel_eine_Farbe(200,Farbwert_berechnen(Akku_Prozent*-1+100),0,Farbwert_berechnen(Akku_Prozent));
}

//Farbwert berechnen aus einem Prozentwert und des maximal Wertes der analog Ausgänge 255
int Farbwert_berechnen(double Prozentwert)
{
	int Ergebnis = 255*Prozentwert/100;
	return Ergebnis;
}

//Neopixelring auf eine Farbe stellen mit einer Wartezeit zwischen dem einzelem Setzen in ms
//Es sind nur abstüfungen von minimal 20ms möglich da Zugriff auf 20ms_Takt
bool NeoPixel_alle_Pixel_eine_Farbe(int Wartezeit, int FarbeR, int FarbeG, int FarbeB)
{
	if (Schalt_Zeitpunkt(Wartezeit,letzter_NeoPixel_Wechsel))
	{
		strip.setPixelColor(aktueller_Pixel,strip.Color(FarbeR,FarbeG,FarbeB));//setzt das aktuelle Pixel auf die gewünschte Farbe
		if (aktueller_Pixel<16)
		{
			aktueller_Pixel=aktueller_Pixel+1;
			letzter_NeoPixel_Wechsel=micros();//Zeit erreicht
		}
		else
		{
			aktueller_Pixel=0;
			letzter_NeoPixel_Wechsel=micros();//Zeit erreicht
			return true;
		}
	}
	return false;
}

//Neopixelring in Startstellung bringen und alle Zähler zurücksetzten
void NeoPixel_Grundstellung()
{
	aktueller_Pixel=0;					//StartPixel auswählen
	letzter_NeoPixel_Wechsel=micros();	//Zeitpunkt auf jetzt stellen
	strip.show();						// Alle Pixel auf aus stellen
}


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

/**
 *      ^   ^   ^  
 * ~~~~~ ColorWave ~~~~~
 *        V   V   V   
 */
void colorWave(uint8_t wait) {
  int i, j, stripsize, cycle;
  float ang, rsin, gsin, bsin, offset;

  static int tick = 0;
  
  stripsize = strip.numPixels();
  cycle = stripsize * 25; // times around the circle...

  while (++tick % cycle) {
    offset = map2PI(tick);

    for (i = 0; i < stripsize; i++) {
      ang = map2PI(i) - offset;
      rsin = sin(ang);
      gsin = sin(2.0 * ang / 3.0 + map2PI(int(stripsize/6)));
      bsin = sin(4.0 * ang / 5.0 + map2PI(int(stripsize/3)));
      strip.setPixelColor(i, strip.Color(trigScale(rsin), trigScale(gsin), trigScale(bsin)));
    }

    strip.show();
    delay(wait);
  }

}

/**
 * Scale a value returned from a trig function to a byte value.
 * [-1, +1] -> [0, 254] 
 * Note that we ignore the possible value of 255, for efficiency,
 * and because nobody will be able to differentiate between the
 * brightness levels of 254 and 255.
 */
byte trigScale(float val) {
  val += 1.0; // move range to [0.0, 2.0]
  val *= 127.0; // move range to [0.0, 254.0]

  return int(val) & 255;
}

/**
 * Map an integer so that [0, striplength] -> [0, 2PI]
 */
float map2PI(int i) {
  return PI*2.0*float(i) / float(strip.numPixels());
}
