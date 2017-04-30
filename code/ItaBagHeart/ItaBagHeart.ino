/*
 * Glowing LED Ita Heart Bag
 * by brightcolorfulflickers
 * April 2017
 *
 * github.com/brightcolorfulflickers
 * brightcolorfulflickers.tumblr.com
 * instagram.com/brightcolorfulflickers
 */

#include "FastLED.h"
#include <Adafruit_CircuitPlayground.h>
#include <TimerOne.h> //this is a library that uses the (16 bit) timer 1 of the arduino to trigger interrupts in certain time intervals.
                     //here we use it to read sensor values precisely every 500ms. 
#ifdef __AVR__
  #include <avr/power.h>
#endif


#define NUM_LEDS 27 
#define NUM_CP      10 
#define COLOR_ORDER GRB
#define DATA_PIN 6 
#define CP_PIN      17
#define NUM_OUTER 18

CRGB leds[NUM_LEDS];
CRGB cp[NUM_CP];

CRGBPalette16 currentPalette;
TBlendType    currentBlending;
uint8_t brightness = 255;  //led strand brightness control
int STEPS;  //makes the rainbow colors more or less spread out

int modeCount = 0;
int numModes = 9;
uint8_t offset = 0;

long lastButtonRead = 0;

volatile unsigned long buttonTime;
volatile byte buttonFlag;//this flag is used to communicate to the main loop that a new value was read.

void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
  Serial.println("Hello!");
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, CP_PIN, COLOR_ORDER>(cp, 10).setCorrection( TypicalLEDStrip );
  currentBlending = LINEARBLEND;
  currentPalette = RainbowColors_p;
  setAll(0x00,0x00,0x00);

  Timer1.initialize(200000); // 200 ms
  Timer1.attachInterrupt(buttonCheck);  
  cp[0] = CRGB( 128,   0,   0);
}

void buttonCheck() {
  int buttonRead = CircuitPlayground.leftButton();//digitalRead(BUTTON);
  if (buttonRead == HIGH) {
    buttonFlag = 1; //set the flag that tells the loop() that a button was pushed
    buttonTime = millis();
    
  }
}

void loop() {

  if (buttonFlag == 1) {
    Serial.println("button!!");
    buttonFlag = 0;
    Serial.println(modeCount);
    modeCount++;
    Serial.println(modeCount);
    Serial.println(modeCount % numModes);
    for (int i = 0; i < NUM_CP; i++) {
      cp[i] = CRGB( 0,   0,   0);  
    }
    cp[modeCount % numModes] = CRGB( 128,   0,   0);
    
  }
   
  if (modeCount % numModes == 0)  {
    //Standard Rainbow
    Serial.println("Rainbow");
    currentPalette = RainbowColors_p;
    STEPS = 3;
    rainbow(100);    
  }
  
  else if (modeCount % numModes == 1)  {
    //Lub Dub
    Serial.println("Lub Dub");
    FadeInnerOuter(0xBB, 0x00, 0x88);   
  }  

  else if (modeCount % numModes == 2)  {
    //Lub Dub
    Serial.println("Pair Skate Lub Dub");
    FadeInnerOuter2Color(0x20, 0x4f, 0x87,70, 0, 59);   
    FadeInnerOuter2Color(70, 0, 59,0x20, 0x4f, 0x87);   
  }  

  else if (modeCount % numModes == 3)  {
    //Rainbow Sparkle
    Serial.println("Rainbow Sparkle");
    currentPalette = RainbowStripeColors_p;
    STEPS = 10;
    rainbow(100); 
  }

  else if (modeCount % numModes == 4)  {
    //Clouds 
    Serial.println("Clouds");
    currentPalette = CloudColors_p;
    STEPS = 7;
    rainbow(100);
  }  
  
  else if (modeCount % numModes == 5)  {
    //PARTY! 
    Serial.println("Party");
    currentPalette = PartyColors_p;
    STEPS = 7;
    rainbow(100);
  }    

  else if (modeCount % numModes == 6)  {
    // White fade entire heart
    Serial.println("White fade");
    FadeInOut(0x80, 0x80, 0x80); 
  }      

  else if (modeCount % numModes == 7)  {
    // Chase!
    Serial.println("Chase");
    chase(0xff, 0x00, 0x00, 3, 75);
  }  
  
  else if (modeCount % numModes == 8)  {
    //Snow Sparkle
    Serial.println("Snow Sparkle");
    SnowSparkle(0x20, 0x20, 0x20, 20, random(100,1000));
  }  



  /*else if (modeCount % numModes == 2)  {
    //Lava
    Serial.println("Lava");
    currentPalette = LavaColors_p;
    STEPS = 7;
    rainbow(100);    
  }*/
}


void chase(byte red, byte green, byte blue, byte tailLength, uint8_t wait) {
  uint16_t i;
  uint16_t j;

  //offset will be index of first pixel lit

    for(i=0; i<NUM_LEDS; i++) {
      byte setR = red/6;
      byte setG = green/6;
      byte setB = blue/6;
      
      if(i <= offset % NUM_LEDS && i >= (offset % NUM_LEDS) - (tailLength-1) ) {
        setR = red;
        setG = green;
        setB = blue;
      }
      setPixel(   i, setR, setG, setB); // First eye
    }
    showStrip();
    offset++;
    delay(wait);
}

void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay) {
  setAll(red,green,blue);
  
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,0xff,0xff,0xff);
  showStrip();
  delay(SparkleDelay);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
}

void rainbow(int delaytime){
  
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
 
  FillLEDsFromPaletteColors( startIndex);
 
  FastLED.show();
  FastLED.delay(delaytime);
}
 
//this bit is in every palette mode, needs to be in there just once
void FillLEDsFromPaletteColors( uint8_t colorIndex) { 
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += STEPS;
    
  }
//  Serial.print(leds[0].red);// + "," + leds[0].green + "," + leds[0].blue);
//  Serial.print(",");
//  Serial.print(leds[0].green);
//  Serial.print(",");
//  Serial.println(leds[0].blue);
}


void FadeInnerOuter(byte red, byte green, byte blue){
  float r, g, b, ir, ig, ib;
      
  for(int k = 0; k < 256; k=k+2) { 
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    ir = ((255-k)/256.0)*red;
    ig = ((255-k)/256.0)*green;
    ib = ((255-k)/256.0)*blue;
    setByIndex(r,g,b,0,NUM_OUTER);
    setByIndex(ir,ig,ib,NUM_OUTER,NUM_LEDS);
    showStrip();
  }
  delay(10);
     
  for(int k = 255; k >= 0; k=k-4) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    ir = ((255-k)/256.0)*red;
    ig = ((255-k)/256.0)*green;
    ib = ((255-k)/256.0)*blue;
    setByIndex(r,g,b,0,NUM_OUTER);
    setByIndex(ir,ig,ib,NUM_OUTER,NUM_LEDS);
    showStrip();
  }
  delay(10);
}

void FadeInnerOuter2Color(byte r1, byte g1, byte b1,byte r2, byte g2, byte b2){
  float r, g, b, ir, ig, ib;
      
  for(int k = 0; k < 256; k=k+2) { 
    r = (k/256.0)*r1;
    g = (k/256.0)*g1;
    b = (k/256.0)*b1;
    ir = ((255-k)/256.0)*r2;
    ig = ((255-k)/256.0)*g2;
    ib = ((255-k)/256.0)*b2;
    setByIndex(r,g,b,0,NUM_OUTER);
    setByIndex(ir,ig,ib,NUM_OUTER,NUM_LEDS);
    showStrip();
  }
  delay(10);
     
  for(int k = 255; k >= 0; k=k-4) {
    r = (k/256.0)*r1;
    g = (k/256.0)*g1;
    b = (k/256.0)*b1;
    ir = ((255-k)/256.0)*r2;
    ig = ((255-k)/256.0)*g2;
    ib = ((255-k)/256.0)*b2;
    setByIndex(r,g,b,0,NUM_OUTER);
    setByIndex(ir,ig,ib,NUM_OUTER,NUM_LEDS);
    showStrip();
  }
  delay(10);
}

void FadeInOut(byte red, byte green, byte blue){
  float r, g, b;
      
  for(int k = 0; k < 256; k=k+1) { 
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
     
  for(int k = 255; k >= 0; k=k-2) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
  
  for (int i=0; i<Count; i++) {
     setPixel(random(NUM_LEDS),random(0,255),random(0,255),random(0,255));
     showStrip();
     delay(SpeedDelay);
     if(OnlyOne) { 
       setAll(0,0,0); 
     }
   }
  
  delay(SpeedDelay);
}

void RGBLoop(){
  for(int j = 0; j < 3; j++ ) { 
    // Fade IN
    for(int k = 0; k < 256; k++) { 
      switch(j) { 
        case 0: setAll(k,0,0); break;
        case 1: setAll(0,k,0); break;
        case 2: setAll(0,0,k); break;
      }
      showStrip();
      delay(3);
    }
    // Fade OUT
    for(int k = 255; k >= 0; k--) { 
      switch(j) { 
        case 0: setAll(k,0,0); break;
        case 1: setAll(0,k,0); break;
        case 2: setAll(0,0,k); break;
      }
      showStrip();
      delay(3);
    }
  }
}

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}

void setByIndex(byte red, byte green, byte blue, byte first, byte last) {
  for(int i = first; i < last; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}

