/*
 * LightStick v1
 * 
 */
#include <Adafruit_NeoPixel.h>

/* PARAMETERS --------------------------------------*/
// Number of rgb in strip.
#define NUM_STRIP 60

// Button Debounce time.
unsigned long debounceDelay = 50;


/* Pin Definitions. --------------------------------*/
// Strip.
#define PIN_STRIP 9
// Buttons. [YELLOW=D6, GREEN=D7]
#define PIN_PAUSE 7
#define PIN_MODE 6
// LEDs.
#define PIN_LED1 5
#define PIN_LED2 4
#define PIN_LED3 3
#define PIN_LED4 2

/* Constants. --------------------------------------*/
// Program Modes.
#define MODE_TOTAL 4
#define MODE_1 0
#define MODE_2 1
#define MODE_3 2
#define MODE_4 3


/* Variables. --------------------------------------*/
// WS2812 Pixels.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_STRIP, PIN_STRIP, NEO_GRB + NEO_KHZ800);

int mode = 0;                 // Program Mode.
bool pause = false;               // Program Colour.

// Button variables.
int bsPause = HIGH;          // current reading from the PAUSE button.
int lbsPause = HIGH;         // previous reading from the PAUSE button.
unsigned long lbtPause = 0;  // Last time the state was toggled.
int bsMode = HIGH;
int lbsMode = HIGH;
unsigned long lbtMode = 0;

int currentStrip = 0;   // current RGB that's on.
unsigned long lastTimeProgram = 0;  // Last time RGB was updated.

void setup() {
  // Debug to console.
  Serial.begin(115200);

  // Set pin modes.
  pinMode(PIN_STRIP, OUTPUT);
  pinMode(PIN_PAUSE, INPUT_PULLUP);
  pinMode(PIN_MODE, INPUT_PULLUP);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  pinMode(PIN_LED4, OUTPUT);

  // Initial mode state.
  mode = MODE_1;
  pause = false;
  setLED(mode, pause);

  strip.begin();
  strip.setBrightness(50);
  strip.show();   // Initialize all pixels to 'off'
}


void loop() {

  button_read();  // Process button actions.
  setLED(mode, pause);   // Change mode indicator.

  // Program Switch.
  switch(mode) {
    case MODE_4:
      handleM4();
      break;
    case MODE_3:
      handleM3();
      break;
    case MODE_2:
      handleM2();
      break;
    case MODE_1:
      handleM1();
      break;
    default:
      // Fault. Revert to MODE_1.
      mode = MODE_1;
      break;
  }

}

// Set the specific LED based on the mode.
void setLED(int m, bool p) {

  switch(m){
    case MODE_4:
      if(p){ 
        digitalWrite(PIN_LED1, HIGH);
        digitalWrite(PIN_LED2, HIGH);
        digitalWrite(PIN_LED3, HIGH);
        digitalWrite(PIN_LED4, LOW); 
      } else { 
        digitalWrite(PIN_LED1, LOW);
        digitalWrite(PIN_LED2, LOW);
        digitalWrite(PIN_LED3, LOW);
        digitalWrite(PIN_LED4, HIGH); 
      }

      break;
    case MODE_3:
      if(p){ 
        digitalWrite(PIN_LED1, HIGH);
        digitalWrite(PIN_LED2, HIGH);
        digitalWrite(PIN_LED3, LOW);
        digitalWrite(PIN_LED4, HIGH); 
      } else { 
        digitalWrite(PIN_LED1, LOW);
        digitalWrite(PIN_LED2, LOW);
        digitalWrite(PIN_LED3, HIGH);
        digitalWrite(PIN_LED4, LOW); 
      }

      break;
    case MODE_2:
      if(p){ 
        digitalWrite(PIN_LED1, HIGH);
        digitalWrite(PIN_LED2, LOW);
        digitalWrite(PIN_LED3, HIGH);
        digitalWrite(PIN_LED4, HIGH); 
      } else { 
        digitalWrite(PIN_LED1, LOW);
        digitalWrite(PIN_LED2, HIGH);
        digitalWrite(PIN_LED3, LOW);
        digitalWrite(PIN_LED4, LOW); 
      }

      break;
    case MODE_1:
      if(p){ 
        digitalWrite(PIN_LED1, LOW);
        digitalWrite(PIN_LED2, HIGH);
        digitalWrite(PIN_LED3, HIGH);
        digitalWrite(PIN_LED4, HIGH); 
      } else { 
        digitalWrite(PIN_LED1, HIGH);
        digitalWrite(PIN_LED2, LOW);
        digitalWrite(PIN_LED3, LOW);
        digitalWrite(PIN_LED4, LOW); 
      }

      break;
    default:
      // Keep all LEDs off.
      break;
  }
  
}

// Set a single RGB at i, coloured c.
void setSingleStrip(uint16_t i, uint32_t c) {

  // Catch invalid RGB.
  if(i < 0 || i >= NUM_STRIP) {
    return;
  }

  // Turn all off.
  for(int x = 0; x < NUM_STRIP; x++) {
    strip.setPixelColor(x, 0);
  }

  // Set rgb at i, and display.
  strip.setPixelColor(i, c);
  strip.show();
}

// Process actions from the buttons.
// Alters 'pause' and 'mode' variables.
void button_read() {

  // Read PIN_PAUSE.
  int readingPause = digitalRead(PIN_PAUSE);

  // Reset the debouncing timer if changed due to noise.
  if(readingPause != lbsPause) {
    lbtPause = millis();
  }

  if((millis() - lbtPause) > debounceDelay) {
    // if button state has changed.
    if(readingPause != bsPause) {
      bsPause = readingPause;

      // Only toggle state if new button state is high.
      if(bsPause == HIGH) {
        // If button is successfully registered,
        pause = !pause;
      }
    }
  }
  // Save the button reading.
  lbsPause = readingPause;


  // Read PIN_MODE.
  int readingMode = digitalRead(PIN_MODE);

  // Reset the debouncing timer if changed due to noise.
  if(readingMode != lbsMode) {
    lbtMode = millis();
  }

  if((millis() - lbtMode) > debounceDelay) {
    // if button state has changed.
    if(readingMode != bsMode) {
      bsMode = readingMode;

      // Only toggle state if new button state is high.
      if(bsMode == HIGH) {
        // If button is successfully registered,
        // Switch modes.
        mode = (mode + 1) % MODE_TOTAL;  // Change mode.
      }
    }
  }
  // Save the button reading.
  lbsMode = readingMode;
}


/*
 * MODE_1 - RED Sequential.
 */
void handleM1() {

  if((millis() - lastTimeProgram) > 1000) {
    lastTimeProgram = millis();

    // Move to next LED.
    if(!pause) {
      currentStrip = (currentStrip + 1) % NUM_STRIP;
    }

    // Turn the LED on.
    setSingleStrip(currentStrip, strip.Color(255,0,0));

  } /* END program delay. */
}

/*
 * MODE_2 - GREEN Sequential.
 */
void handleM2() {

  if((millis() - lastTimeProgram) > 1000) {
    lastTimeProgram = millis();

    // Move to next LED.
    if(!pause) {
      currentStrip = (currentStrip + 1) % NUM_STRIP;
    }

    // Turn the LED on.
    setSingleStrip(currentStrip, strip.Color(0,255,0));

  } /* END program delay. */
}

/*
 * MODE_3 - RED Reverse Sequential.
 */
void handleM3() {

  if((millis() - lastTimeProgram) > 1000) {
    lastTimeProgram = millis();

    // Move to next LED.
    if(!pause) {
      currentStrip = (NUM_STRIP + currentStrip - 1) % NUM_STRIP;
    }

    // Turn the LED on.
    setSingleStrip(currentStrip, strip.Color(255,0,0));

  } /* END program delay. */
}

/*
 * MODE_4 - GREEN Reverse Sequential.
 */
void handleM4() {

  if((millis() - lastTimeProgram) > 1000) {
    lastTimeProgram = millis();

    // Move to next LED.
    if(!pause) {
      currentStrip = (NUM_STRIP + currentStrip - 1) % NUM_STRIP;
    }

    // Turn the LED on.
    setSingleStrip(currentStrip, strip.Color(0,255,0));

  } /* END program delay. */
}
