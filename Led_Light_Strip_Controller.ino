#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_STRIP_PIN 13
#define NUM_LEDS 144
int BRIGHTNESS = 255;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_STRIP_PIN, NEO_GRBW + NEO_KHZ800);

struct LEDControllerPin
{
  
  public:
  
  LEDControllerPin(byte ctrl, byte led)
  {
    controllerPin   = ctrl;
    ledPin          = led;    
  }
  
  void setup()
  {
    pinMode(ledPin,        OUTPUT);
    pinMode(controllerPin, INPUT);
  }
  
  bool isToggled()
  {
    if(digitalRead(controllerPin) == LOW)
    {
      digitalWrite(ledPin, HIGH);   
      return true;
    }
    
    digitalWrite(ledPin, LOW);
    return false;
    
  }
     
  private: 
  
  byte controllerPin;
  byte ledPin;
  
 
  
};


LEDControllerPin leftPin     = LEDControllerPin(5, 8);
LEDControllerPin downPin     = LEDControllerPin(3, 10);
LEDControllerPin upPin       = LEDControllerPin(4, 9);
LEDControllerPin rightPin    = LEDControllerPin(2, 11);
LEDControllerPin centerPin   = LEDControllerPin(6, 12);



enum LightMode
{
  White         = 0,
  Rainbow       = 1,
  WipeRGBW      = 2,
  WipeRGB       = 3,
  WipeRB        = 4,
  RainbowAnalog = 5,
  LightModeMax  = 6
};

float currentLightMode     = White;
byte currentLightModebyte  = White;
byte lastLightModebyte     = White;

void setup() 
{ 
  Serial.begin(9600);
  leftPin.setup();
  
  
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
}


byte detectInput()
{
    if(leftPin.isToggled  ())
    {
      currentLightMode -= .075;
      
      if(currentLightMode < 0)
      {
        currentLightMode = LightModeMax;
      }
    }
    
    if(rightPin.isToggled ())
    {
      currentLightMode += .075;
      
      if(currentLightMode >= LightModeMax)
      {
        currentLightMode = 0;
      }
    }
    
    
    
    if(downPin.isToggled  ())
    {
      if(BRIGHTNESS - 5 > 0)
      {
        BRIGHTNESS -= 5;
        strip.setBrightness(BRIGHTNESS);
        strip.show();
      }
    }
    
    if(upPin.isToggled())
    {
      if(BRIGHTNESS + 5 < 255)
      {
        BRIGHTNESS += 5;
        strip.setBrightness(BRIGHTNESS);
        strip.show();
      }
    }
    
    centerPin.isToggled();
    
    
    
    currentLightModebyte = currentLightMode;  //float -> byte
  
    if(lastLightModebyte != currentLightModebyte)
    {
      lastLightModebyte = currentLightModebyte;
      return true;
    }
    
    return false;
    
}


void whiteLight(byte wait)
{
  
    for(uint16_t i=0; i<strip.numPixels(); i++) 
    {
        strip.setPixelColor(i, strip.Color(0,0,0, 255 ));
    }
    strip.show();
      
    while(true)
    {
      if(detectInput())
      {
        return;
      }
    
      delay(wait);
    }
    
}

void rainbowLight(byte wait)
{
  
  while(true)
  {
    uint16_t i, j;
  
    for(j=0; j < 256; j++) 
    {
      for(i=0; i< strip.numPixels(); i++)
      {
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      }
      strip.show();
      if(detectInput())
      {
        return;
      }
      
      delay(wait);
    }
    
  }
  
}

void rainbowLightAnalog(byte wait)
{
  
  while(true)
  {
    uint16_t i, j;
  
    for(j=0; j < 256; j++) 
    {
      for(i=0; i< strip.numPixels(); i++)
      {
        strip.setPixelColor(i, Wheel(((256 / strip.numPixels()) + j) & 255));
      }
      strip.show();
      if(detectInput())
      {
        return;
      }
      
      delay(wait);
    }
    
  }
  
}


void setRandom(byte wait)
{
  
  while (true)
  {
    
    byte amount = random(NUM_LEDS);
    
    
    for(byte i = 0; i < amount; i++)
    {
      strip.setPixelColor(random(NUM_LEDS), strip.Color(random(255),random(255),random(255), random(255)));
    }
    
    if(detectInput())
    {
      return;
    }
  
    delay(wait);
    
  }
  
  
}

void wipeRGBW(byte wait)
{
  while (true)
  {
    
    if(colorWipe(0, 0, 0, 255, wait)){return;}
    if(colorWipe(255, 0, 0, 0, wait)){return;}
    if(colorWipe(0, 255, 0, 0, wait)){return;}
    if(colorWipe(0, 0, 255, 0, wait)){return;}   

  }
}
void wipeRGB(byte wait)
{
  while (true)
  {
    
    if(colorWipe(255, 0, 0, 0, wait)){return;}
    if(colorWipe(0, 255, 0, 0, wait)){return;}
    if(colorWipe(0, 0, 255, 0, wait)){return;}   

  }
}
void wipeRB(byte wait)
{
  while (true)
  {
    
    if(colorWipe(0, 0, 255, 0, wait)){return;}
    if(colorWipe(255, 0, 0, 0, wait)){return;}

  }
}

void loop() 
{
  
  switch((LightMode)currentLightModebyte)
  {
    case White:
    default:
    
    whiteLight(50);
    break;
    
    case Rainbow:
    rainbowLight(50);
    break;
    
    case WipeRGBW:
    wipeRGBW(50);
    break;
    
    case WipeRB:
    wipeRB(50);
    break;
    
    case WipeRGB:
    wipeRGB(50);
    break;

    case RainbowAnalog:
    rainbowLightAnalog(50);
    break;
    
  }  



}

// Fill the dots one after the other with a color
byte colorWipe(byte r, byte g, byte b, byte w, uint8_t wait) 
{
  uint32_t color = strip.Color((r * BRIGHTNESS) / 255, (g * BRIGHTNESS) / 255, (b * BRIGHTNESS) / 255, (w * BRIGHTNESS) / 255);
  
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    
    if(detectInput())
    {
      return true;
    }
    
    delay(wait);
    
  }
  
  return false;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) 
{
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3,0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3,0);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0,0);
}

uint8_t red(uint32_t c) {
  return (c >> 16);
}
uint8_t green(uint32_t c) {
  return (c >> 8);
}
uint8_t blue(uint32_t c) {
  return (c);
}

