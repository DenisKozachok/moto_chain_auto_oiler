#include <Arduino.h>

const int buttonPin = 15;
const int pumpPin = 5;
const int ledPin = 2;

enum Mode { OFF, ON, PURGE };
Mode currentMode = OFF;

unsigned long purgeStartTime = 0;
bool purgeRunning = false;

unsigned long lastButtonPress = 0;
int clickCount = 0;
const unsigned long doubleClickThreshold = 400;

void applyMode() {
  switch (currentMode) {
    case OFF:
      digitalWrite(pumpPin, LOW);
      digitalWrite(ledPin, LOW);
      Serial.println("Mode: OFF");
      break;

    case ON:
      digitalWrite(pumpPin, HIGH);
      digitalWrite(ledPin, HIGH);
      Serial.println("Mode: ON");
      break;

    case PURGE:
      purgeStartTime = millis();
      purgeRunning = true;
      digitalWrite(pumpPin, HIGH);
      Serial.println("Mode: PURGE (30s)");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(pumpPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  applyMode();
}

void loop() {
  static bool lastButtonState = HIGH;
  static unsigned long debounceTime = 0;
  bool buttonState = digitalRead(buttonPin);
  unsigned long now = millis();

  // Button handling with debounce
  if (lastButtonState == HIGH && buttonState == LOW && (now - debounceTime > 200)) {
    debounceTime = now;

    if (now - lastButtonPress < doubleClickThreshold) {
      // Double click
      clickCount++;
    } else {
      // Too long — treat as a new session
      clickCount = 1;
    }

    lastButtonPress = now;
  }

  // Click handling
  if (clickCount > 0 && (now - lastButtonPress > doubleClickThreshold)) {
    if (clickCount == 1) {
      // Single click — toggle OFF ↔ ON
      currentMode = (currentMode == OFF) ? ON : OFF;
      applyMode();
    } else if (clickCount == 2) {
      // Double click — activate PURGE
      currentMode = PURGE;
      applyMode();
    }

    clickCount = 0; // reset
  }

  lastButtonState = buttonState;

  // PURGE mode handling
  if (currentMode == PURGE && purgeRunning) {
    digitalWrite(ledPin, (now / 500) % 2);

    if (now - purgeStartTime >= 30000) {
      currentMode = ON;
      purgeRunning = false;
      applyMode();
    }
  }
}

// void setup() {
//   Serial.begin(9600); // Baud rate
//   Serial.println("ESP32 is starting...");
//   pinMode(2, OUTPUT); // Set GPIO2 as output
// }

// void loop() {
//   digitalWrite(2, HIGH); // Turn on LED
//   delay(1000);           // Wait 1 second
//   digitalWrite(2, LOW);  // Turn off LED
//   delay(1000);           // Wait 1 second

//   for (int i = 0; i < 10; i++) {
//     digitalWrite(2, HIGH); // Turn on LED
//     delay(300);            // Wait 300 ms
//     digitalWrite(2, LOW);  // Turn off LED
//     delay(300);            // Wait 300 ms
//     Serial.println(i);     // Print numbers from 0 to 9
//   }
//             // Wait 1 second
//}
