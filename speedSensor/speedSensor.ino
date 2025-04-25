#include <Arduino.h>

// Pin configuration
const int hallPin = 34;       // analogRead pin for Hall sensor
const int threshold = 3000;   // Trigger threshold for magnet detection
const int ledPin = 2;         // Built-in LED on ESP32 (typically GPIO2)

// Wheel parameters
const float wheelDiameter = 0.6; // Diameter of the wheel in meters

// State variables
int rotationCount = 0;
bool magnetDetected = false;
unsigned long lastCalculationTime = 0;

void printStartupInfo() {
  Serial.println("🚀 System initialized. Waiting for impulses from Hall sensor 49E...");
  Serial.print("📊 Threshold set to: ");
  Serial.println(threshold);
  Serial.print("🔁 Wheel diameter: ");
  Serial.print(wheelDiameter);
  Serial.println(" meters");
  Serial.println("----------------------------------------");
}

void printCurrentValue(int value) {
  Serial.print("Current value: ");
  Serial.print(value);
  Serial.print(" | Threshold: ");
  Serial.print(threshold);
}

void printSpeed(float speed) {
  Serial.println("----------------------------------------");
  Serial.print("📈 Rotations per second: ");
  Serial.print(rotationCount);
  Serial.print(" | 🚗 Speed: ");
  Serial.print(speed, 2);
  Serial.println(" km/h");
  Serial.println("----------------------------------------");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  printStartupInfo();
}

void loop() {
  int sensorValue = analogRead(hallPin);
  unsigned long currentTime = millis();

  printCurrentValue(sensorValue);

  if (sensorValue > threshold) {
    Serial.println(" 🔍 > Threshold");

    digitalWrite(ledPin, HIGH); // Turn on LED when magnet is detected

    if (!magnetDetected) {
      rotationCount++;
      magnetDetected = true;
      Serial.println("✅ Magnet detected! LED is ON.");
    }
  } else {
    Serial.println(" < Threshold");

    digitalWrite(ledPin, LOW); // Turn off LED if magnet is not detected
    magnetDetected = false;
  }

  // Calculate speed every second
  if (currentTime - lastCalculationTime >= 1000) {
    float wheelCircumference = 3.14 * wheelDiameter;
    float speed = (rotationCount * wheelCircumference * 3600.0) / 1000.0; // km/h

    printSpeed(speed);

    rotationCount = 0;
    lastCalculationTime = currentTime;
  }

  delay(150); // Delay to reduce log output frequency
}


// const int hallPin = 34; // analogRead пин
// const int threshold = 3000; // Порог для фиксации магнитного поля
// float wheelDiameter = 0.6; // Диаметр колеса в метрах
// int count = 0;
// bool magnetDetected = false;
// unsigned long lastCalcTime = 0;

// void setup() {
//   Serial.begin(115200);
// }

// void loop() {
//   int value = analogRead(hallPin);
//   unsigned long currentTime = millis();

//   if (value > threshold && !magnetDetected) {
//     count++;
//     magnetDetected = true;
//     Serial.println("Магнит прошёл");
//   }

//   if (value < threshold) {
//     magnetDetected = false;
//   }

//   // Каждую секунду считаем скорость
//   if (currentTime - lastCalcTime >= 1000) {
//     float wheelCircumference = 3.14 * wheelDiameter;
//     float speed = (count * wheelCircumference * 3600) / 1000.0; // км/ч

//     Serial.print("Оборотов за сек: ");
//     Serial.print(count);
//     Serial.print("  Скорость: ");
//     Serial.print(speed);
//     Serial.println(" км/ч");

//     count = 0;
//     lastCalcTime = currentTime;
//   }
// }



// const int hallPin = 34; // Analog pin
// const int ledPin = 2;   // Встроенный светодиод

// void setup() {
//   Serial.begin(115200);
//   pinMode(ledPin, OUTPUT);
// }

// void loop() {
//   int value = analogRead(hallPin);
//   Serial.print("Аналоговое значение: ");
//   Serial.println(value);

//   if (value > 3000) {
//     digitalWrite(ledPin, HIGH);
//     Serial.println("Магнит обнаружен! Светодиод включен.");
//   } else {
//     digitalWrite(ledPin, LOW);
//     Serial.println("Магнита нет. Светодиод выключен.");
//   }

// //  delay(200);
// }
