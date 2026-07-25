#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
#define MQ135_PIN      A0
#define FLAME_PIN      2
#define DHT_PIN        3
#define BUTTON_PIN     4   // Using button instead of touch sensor
#define BUZZER_PIN     5
#define CHARGE_LED_PIN 6

// DHT Sensor Config
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// Thresholds
const int GAS_THRESHOLD = 400; 
const float TEMP_LIMIT = 45.0; 

// System States
bool systemActive = false;
bool thermalEvent = false;
bool lastButtonState = HIGH; // track button press

unsigned long lastDHTRead = 0;
float tempC = 25.0;

void setup() {
  Serial.begin(9600);

  pinMode(FLAME_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Button with pull-up resistor
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(CHARGE_LED_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(CHARGE_LED_PIN, LOW);

  dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.println(F("EV BAY SYSTEM"));
  display.setCursor(10, 35);
  display.println(F("INITIALIZING..."));
  display.display();
  delay(2000);
}

void loop() {
  // --- Button Edge Detection ---
  bool buttonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && buttonState == LOW) {
    // Button just pressed
    if (thermalEvent) {
      thermalEvent = false;
      systemActive = false;
      noTone(BUZZER_PIN);
    } else {
      systemActive = !systemActive;
    }
    delay(300); // debounce
  }
  lastButtonState = buttonState;

  // --- Read Sensors ---
  int gasLevel = analogRead(MQ135_PIN);
  int flameStatus = digitalRead(FLAME_PIN);

  // Read DHT only every 2s
  if (millis() - lastDHTRead > 2000) {
    float t = dht.readTemperature();
    if (!isnan(t)) tempC = t;
    lastDHTRead = millis();
  }

  // Debug prints
  Serial.print("Gas: "); Serial.print(gasLevel);
  Serial.print(" | Flame: "); Serial.print(flameStatus);
  Serial.print(" | Temp: "); Serial.print(tempC);
  Serial.print(" | Button: "); Serial.println(buttonState);

  // --- Safety Checks ---
  bool flameDetected = (flameStatus == LOW); 
  bool gasAlert = (gasLevel > GAS_THRESHOLD);
  bool tempAlert = (tempC > TEMP_LIMIT);

  if ((flameDetected || gasAlert || tempAlert) && systemActive) {
    thermalEvent = true;
  }

  // --- CONTROL LOGIC ---
  if (thermalEvent) {
    digitalWrite(CHARGE_LED_PIN, LOW); 
    tone(BUZZER_PIN,1000);

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 5);
    display.println(F(" CRITICAL!"));
    display.setTextSize(1);
    display.setCursor(0, 30);
    display.println(F("THERMAL EVENT DETECTED"));
    display.setCursor(0, 45);
    display.println(F("POWER CUT OFF"));
    display.display();
  } 
  else if (systemActive) {
    digitalWrite(CHARGE_LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(F("STATUS: CHARGING..."));
    display.drawLine(0, 10, 128, 10, WHITE);
    
    display.setCursor(0, 20);
    display.print(F("Temp: ")); display.print(tempC, 1); display.println(F(" C"));
    
    display.setCursor(0, 35);
    display.print(F("Gas:  ")); display.print(gasLevel); display.println(F(" PPM"));
    
    display.setCursor(0, 50);
    display.print(F("Flame: ")); display.print(flameDetected ? "DETECTED" : "OK");
    display.display();
  } 
  else {
    digitalWrite(CHARGE_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 10);
    display.println(F("   EV CHARGING BAY"));
    display.drawLine(0, 22, 128, 22, WHITE);
    display.setCursor(15, 35);
    display.println(F("SYSTEM STANDBY"));
    display.setCursor(0, 50);
    display.println(F("Press button to start"));
    display.display();
  }
}
