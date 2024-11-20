#include <FastLED.h>
#include <IRremote.h>

#define NUM_LEDS 16
#define DATA_PIN 10
#define DATA_PIN_OUT 5
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define IR_PIN 7

uint16_t BRIGHTNESS = 255;

IRrecv receiver(IR_PIN);
uint8_t decodedData = 162; // 162 is LEDs off; change this value to alter the default
uint8_t plusOrMinus = 0;

CRGB leds[NUM_LEDS];

// Faster way to change RGB values of LED; otherwise every
// regular color would require its own for loop
void LEDOn(int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(r, g, b);
  }
}

// Fade out a single LED
void LEDFadeWhiteToBlack(int ledNum) {
  for (int i = 255; i > 0; i = i - 8) {   // Change i = i - x to alter speed
    leds[ledNum] = CRGB(i, i, i);
    FastLED.show();
  }
  // Ensures the LED is completely off, can get screwy with different x values
  leds[ledNum] = CRGB(0, 0, 0);
  FastLED.show();
}

// Fade in a single LED
void LEDFadeBlackToWhite(int ledNum) {
  for (int i = 0; i < 255; i = i + 8) {   // Change i = i + x to alter speed
    leds[ledNum] = CRGB(i, i, i);
    FastLED.show();
  }
  // Ensures the LED is completely on, can get screwy with different x values
  leds[ledNum] = CRGB(255, 255, 255);
  FastLED.show();
}

uint16_t starCount = 0;   // Number of "stars"
uint8_t fadeCounter = 0;  // Used to determine whether or not we fade out
bool starModeInitializer = false;
void starMode() {
  // Initializer
  if (starModeInitializer) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    starModeInitializer = false;
  }

  // Randomizers
  uint16_t randNumLED = random(NUM_LEDS);   // Random LED to fade into
  uint8_t randNum = random(50);             // Used to see whether or not we fade this LED in
  uint16_t randNum2 = random(1000);         // Used to determine whether or not we shooting star

  // Fades LEDs out
  // Change the fadeCounter == x to alter how often this code will run
  if (fadeCounter == 2) {
    fadeToBlackBy(leds, NUM_LEDS, 1);
    fadeCounter = 0;
  }
  fadeCounter++;


  // Adds stars
  if (randNum == 1) {
    LEDFadeBlackToWhite(randNumLED);
    starCount++;
  }

  //Shooting star
  if (randNum2 == 1) {
    for (int i = 0; i < NUM_LEDS; i++) {
      // This if statement will make it so the shooting star doesn't clear existing stars
      if (leds[i] == CRGB(0, 0, 0)) {
        LEDFadeBlackToWhite(i);
        delay(100);
        LEDFadeWhiteToBlack(i);
      }
      else {
        delay(120);   // Used so when the shooting star skips it won't teleport
      }

      // Breaks if user inputs a new ir
      if (receiver.decode()) {
        if (receiver.decodedIRData.command != 66) {
          decodedData = receiver.decodedIRData.command;
          receiver.resume();
          break;
        }
      }
    }
  }
}

// If this is put directly into the switch statement then it will cause issues
void rainbowMode(uint8_t speed, uint8_t deltaHue) {
  uint8_t thisHue = beat8(speed, 255);
  fill_rainbow(leds, NUM_LEDS, thisHue, deltaHue);
}

void decodeData() {
  if (receiver.decode()) {
    if (receiver.decodedIRData.command == 2 || receiver.decodedIRData.command == 152) {
      plusOrMinus = receiver.decodedIRData.command;
    }
    // If LED is already in star mode, it will not be reset to star mode initializer
    else if (receiver.decodedIRData.command == 66 && decodedData == 67) {}
    else {
      decodedData = receiver.decodedIRData.command;
    }
    receiver.resume();  // Receive the next value
  }
}

void changeEffect() {
  switch (decodedData) {
    case 162: // Power off
      LEDOn(0, 0, 0);
      break;
    case 48:  // 1 red
      LEDOn(255, 0, 0);
      break;
    case 24:  // 2 green
      LEDOn(0, 255, 0);
      break;
    case 122: // 3 blue
      LEDOn(0, 0, 255);
      break;
    case 16:  // 4 purple
      LEDOn(165, 0, 165);
      break;
    case 56:  // 5 white
      LEDOn(255, 255, 255);
      break;
    case 90:  // 6 Christmas
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        if (i % 2 == 0) {
          leds[i] = CRGB(255, 0, 0);
        }
        if (i % 2 == 1) {
          leds[i] = CRGB(0, 255, 0);
        }
      }
      break;
    case 66:  // 7 starmode initializer
      starModeInitializer = true;
      decodedData++;
      break;
    case 67:  // 7 starmode, remote will never directly input this value, initializer will always run
      starMode();
      break;
    case 74:  // 8 rainbowmode
      rainbowMode(10, 10);
      break;
    case 82:  // 9 light blue
      LEDOn(0, 195, 255);
      break;
  }
}

void changeBrightness() {
  switch (plusOrMinus) {
    case 2:   // + brightness up
      BRIGHTNESS += 12;
      if (BRIGHTNESS > 255) {
        BRIGHTNESS = 255;
      }

      FastLED.setBrightness(BRIGHTNESS);
      plusOrMinus = 0;
      break;
    case 152: // - brightness down
      BRIGHTNESS -= 12;
      if (BRIGHTNESS < 96) {
        BRIGHTNESS = 96;
      }

      FastLED.setBrightness(BRIGHTNESS);
      plusOrMinus = 0;
      break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.begin(DATA_PIN_OUT);

  receiver.enableIRIn();

  pinMode(DATA_PIN_OUT, OUTPUT);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
  digitalWrite(DATA_PIN_OUT, HIGH);

  randomSeed(analogRead(0));
  
  delay(3000);
}

void loop() {
  decodeData();
  changeEffect();
  changeBrightness();

  FastLED.show();
  delay(20);  // Small delay will improve performance
}
