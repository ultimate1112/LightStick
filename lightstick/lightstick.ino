/*
 * LightStick v1
 * 
 */
#include <Adafruit_NeoPixel.h>

/* PARAMETERS --------------------------------------*/
// Number of rgb in strip.
#define NUM_STRIP 10

// Button Debounce time.
unsigned long debounceDelay = 50;

unsigned long programDelay = 1000;

/* Pin Definitions. --------------------------------*/
// Strip.
#define PIN_STRIP 9
// Buttons. [YELLOW=D6, GREEN=D7]
#define PIN_COLOUR 7
#define PIN_MODE 6
// LEDs.
#define PIN_LED1 5
#define PIN_LED2 4
#define PIN_LED3 3
#define PIN_LED4 2

/* Constants. --------------------------------------*/
// Program Modes.
#define MODE_TOTAL 4
#define MODE_OFF 0
#define MODE_SEQ 1
#define MODE_RANDOM 2
#define MODE_ALT 3

// Program Colours.
#define COLOUR_TOTAL 4
#define COLOUR_RED 0
#define COLOUR_GREEN 1
#define COLOUR_BLUE 2
#define COLOUR_RANDOM 3

/* Variables. --------------------------------------*/
// WS2812 Pixels.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_STRIP, PIN_STRIP, NEO_GRB + NEO_KHZ800);

int mode = 0;                 // Program Mode.
int colour = 0;               // Program Colour.

// Button variables.
int bsColour = HIGH;          // current reading from the colour button.
int lbsColour = HIGH;         // previous reading from the colour button.
unsigned long lbtColour = 0;  // Last time the state was toggled.
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
  pinMode(PIN_COLOUR, INPUT_PULLUP);
  pinMode(PIN_MODE, INPUT_PULLUP);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  pinMode(PIN_LED4, OUTPUT);

  // Initial mode state.
  mode = MODE_OFF;
  colour = COLOUR_RED;
  setLED(mode);

  strip.begin();
  strip.setBrightness(50);
  strip.show();   // Initialize all pixels to 'off'
}


void loop() {

  button_read();  // Process button actions.
  setLED(mode);   // Change mode indicator.

  // Program.
  if((millis() - lastTimeProgram) > programDelay) {
    lastTimeProgram = millis();
    Serial.println('y');
    switch(mode) {
      case MODE_ALT:
      
        break;
      case MODE_RANDOM:
        handleRandom(); 
        break;
      case MODE_SEQ:
        handleSeq();      
        break;
      case MODE_OFF:
      default:
        // Keep all RGBs off.
        handleOff();
        break;
    }
  }

}

// Set the specific LED based on the mode.
void setLED(int m) {

  // Switch all LEDs off.
  digitalWrite(PIN_LED1, LOW);
  digitalWrite(PIN_LED2, LOW);
  digitalWrite(PIN_LED3, LOW);
  digitalWrite(PIN_LED4, LOW);

  switch(m){
    case MODE_ALT:
      digitalWrite(PIN_LED3, HIGH);
      break;
    case MODE_RANDOM:
      digitalWrite(PIN_LED2, HIGH);
      break;
    case MODE_SEQ:
      digitalWrite(PIN_LED1, HIGH);
      break;
    case MODE_OFF:
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
// Alters 'colour' and 'mode' variables.
void button_read() {

  // Read PIN_COLOUR.
  int readingColour = digitalRead(PIN_COLOUR);

  // Reset the debouncing timer if changed due to noise.
  if(readingColour != lbsColour) {
    lbtColour = millis();
  }

  if((millis() - lbtColour) > debounceDelay) {
    // if button state has changed.
    if(readingColour != bsColour) {
      bsColour = readingColour;

      // Only toggle state if new button state is high.
      if(bsColour == HIGH) {
        // If button is successfully registered,
        // Switch colours.
        colour = (colour + 1) % COLOUR_TOTAL; // Change colour.
      }
    }
  }
  // Save the button reading.
  lbsColour = readingColour;


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

// Handle the off mode.
void handleOff() {
  for(int x = 0; x < NUM_STRIP; x++) {
    strip.setPixelColor(x, 0);
  }
  strip.show();
}




// Handle the sequential mode.
void handleSeq() {
  
  uint32_t c;

  switch(colour) {
    case COLOUR_RANDOM:
      c = strip.Color(random(0,255),
          random(0,255),
          random(0,255));
      break;
    case COLOUR_BLUE:
      c = strip.Color(0,0,255);
      break;
    case COLOUR_GREEN:
      c = strip.Color(0,255,0);
      break;
    case COLOUR_RED:
    default:
      c = strip.Color(255,0,0);
  }

  setSingleStrip(currentStrip, c);
  // Move to next rgb.
  currentStrip = (currentStrip + 1) % NUM_STRIP;
}

// Handle the random mode.
void handleRandom() {
  
  uint32_t c;

  switch(colour) {
    case COLOUR_RANDOM:
      c = strip.Color(random(0,255),
          random(0,255),
          random(0,255));
      break;
    case COLOUR_BLUE:
      c = strip.Color(0,0,255);
      break;
    case COLOUR_GREEN:
      c = strip.Color(0,255,0);
      break;
    case COLOUR_RED:
    default:
      c = strip.Color(255,0,0);
  }

  // Generate Random RGB.
  currentStrip = random(0, NUM_STRIP);
  setSingleStrip(currentStrip, c);
}
