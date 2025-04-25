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

  // Обработка кнопки с антидребезгом
  if (lastButtonState == HIGH && buttonState == LOW && (now - debounceTime > 200)) {
    debounceTime = now;

    if (now - lastButtonPress < doubleClickThreshold) {
      // Двойной клик
      clickCount++;
    } else {
      // Слишком долго — считаем это новой сессией
      clickCount = 1;
    }

    lastButtonPress = now;
  }

  // Обработка кликов
  if (clickCount > 0 && (now - lastButtonPress > doubleClickThreshold)) {
    if (clickCount == 1) {
      // Одинарное нажатие — переключение OFF ↔ ON
      currentMode = (currentMode == OFF) ? ON : OFF;
      applyMode();
    } else if (clickCount == 2) {
      // Двойное нажатие — запускаем PURGE
      currentMode = PURGE;
      applyMode();
    }

    clickCount = 0; // сброс
  }

  lastButtonState = buttonState;

  // Обработка режима PURGE
  if (currentMode == PURGE && purgeRunning) {
    digitalWrite(ledPin, (now / 500) % 2);

    if (now - purgeStartTime >= 30000) {
      currentMode = ON;
      purgeRunning = false;
      applyMode();
    }
  }
}


















































// #include <Arduino.h>

// const int buttonPin = 15;

// enum Mode { STANDARD, OFFROAD, SATURATION, PURGE };
// Mode currentMode = STANDARD;
// unsigned long lastSwitchTime = 0;

// void printMode() {
//   switch (currentMode) {
//     case STANDARD: Serial.println("Mode: STANDARD"); break;
//     case OFFROAD: Serial.println("Mode: OFFROAD"); break;
//     case SATURATION: Serial.println("Mode: SATURATION"); break;
//     case PURGE: Serial.println("Mode: PURGE"); break;
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   pinMode(buttonPin, INPUT_PULLUP);
//   printMode();
// }

// void loop() {
//   static bool lastButtonState = HIGH;
//   bool buttonState = digitalRead(buttonPin);

//   if (lastButtonState == HIGH && buttonState == LOW && millis() - lastSwitchTime > 500) {
//     lastSwitchTime = millis();
//     currentMode = (Mode)((currentMode + 1) % 4);
//     printMode();
//   }
//   lastButtonState = buttonState;
// }

// void setup() {
//   Serial.begin(9600); // Скорость передачи данных
//   Serial.println("ESP32 стартует...");
//   pinMode(2, OUTPUT); // Устанавливаем GPIO2 как выход
// }

// void loop() {
//   digitalWrite(2, HIGH); // Включаем светодиод
//   delay(1000);           // Ждём 1 секунду
//   digitalWrite(2, LOW);  // Выключаем светодиод
//   delay(1000);           // Ждём 1 секунду
  
//   for (int i = 0; i < 10; i++) {
//     digitalWrite(2, HIGH); // Включаем светодиод
//     delay(300);           // Ждём 1 секунду
//     digitalWrite(2, LOW);  // Выключаем светодиод
//     delay(300);           // Ждём 1 секунду
//     Serial.println(i); // выведет числа от 0 до 9
//   }
//             // Ждём 1 секунду
//}
