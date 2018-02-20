#include <SerialCraft.h>
#include <Adafruit_NeoPixel.h>

// Change these variables to match your setup
// number of LEDs in your strip
#define NUMLEDS 8

// pin that LED data pin is connected to
#define PIN 12

// number of blocks that each LED represents
#define BLOCKS_PER_LED 4

// The type of LED strip you have (if your LEDs aren't turning green, then you'll need to change the order of the GRBW)
#define LED_TYPE (NEO_GRBW+NEO_KHZ800)

// END variables

// These are values that will be used in our calculations for LED brightness
// distance creeper will begin detonating
#define DETONATE_DIST 3

// distance we are safe from a creeper explosion (you'll take damage if you're within this distance)
#define SAFE_DIST 7

// max distance that we track a creeper
#define MAX_DIST (NUMLEDS*BLOCKS_PER_LED)

// This is the SerialCraft object for communicating with the SerialCraft Minecraft mod
SerialCraft sc;

// distance from creeper
int dist = 100;

// Initialize a strip of LEDs, you may need to change the 3rd
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMLEDS, PIN, LED_TYPE);

void setup() {
  // initialize SerialCraft
  sc.setup();
  // register a creeper distance callback to receive the distance to the nearest creeper
  sc.registerCreeperDistanceCallback(creeper);

  // initialize the LED strip
  strip.begin();
  strip.show();
}

void loop() {
  // ranges from 0 when >= MAX_DIST away from creeper's detonation radius to NUMLEDS*BLOCKS_PER_LED when on top of creeper
  int blocksFromCreeperToMax = constrain(MAX_DIST+DETONATE_DIST-dist, 0, MAX_DIST);
  
  int curLED = blocksFromCreeperToMax/BLOCKS_PER_LED; // ranges from 0 to NUMLEDS-1
  int curLEDLevel = (blocksFromCreeperToMax%BLOCKS_PER_LED+1); // ranges from 1 to BLOCKS_PER_LED

  for(uint16_t i = 0; i < strip.numPixels(); i++) {
    if(i <= curLED) {
      // brightest when within creeper's detonation radius, off when creeper is NUMLEDS*BLOCKS_PER_LED away
      float intensity = (float)blocksFromCreeperToMax/MAX_DIST;
    
      if(i == curLED) { // last LED lit
        // make last LED brighter as we approach the next LED
        float lastIntensity = (float)curLEDLevel/BLOCKS_PER_LED;
        intensity *= lastIntensity;
      }

      if(dist < SAFE_DIST) {
        intensity *= (millis()/75)%2;
      }
      
      intensity = pow(intensity, 2.2); // gamma curve, makes the LED brightness look linear to our eye when the brightness value really isn't
  
      strip.setPixelColor(i, strip.Color(10*intensity, 70*intensity, 10*intensity, 0));
    } else {
      strip.setPixelColor(i, strip.Color(0,0,0,0));
    }
  }
  strip.show();
  sc.loop();
}

void creeper(int d) {
  dist = d;
}

