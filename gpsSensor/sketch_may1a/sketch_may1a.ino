#include <HardwareSerial.h>

HardwareSerial gpsSerial(1);
const int RXPin = 16;  // GPS TX → сюда
const int TXPin = 17;  // GPS RX (можно не подключать)

String gpsLine = "";
bool gpsFixed = false;

const int ledPin = 2;            // Встроенный светодиод ESP32 (GPIO2)
unsigned long lastBlink = 0;
const unsigned long blinkInterval = 10000;  // 10 секунд

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, RXPin, TXPin);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println("📡 GPS парсер с диодом запущен...");
}

void loop() {
  // Чтение GPS
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    if (c == '\n') {
      gpsLine.trim();

      if (gpsLine.startsWith("$GNRMC")) {
        parseGNRMC(gpsLine);
      } else if (gpsLine.startsWith("$GNGGA")) {
        parseGNGGA(gpsLine);
      }

      gpsLine = "";
    } else {
      gpsLine += c;
    }
  }

  // Индикация LED каждые 10 секунд, если нет сигнала
  if (!gpsFixed) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastBlink >= blinkInterval) {
      lastBlink = currentMillis;
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
    }
  } else {
    digitalWrite(ledPin, HIGH); 
  }
}

void parseGNRMC(String rmc) {
  String tokens[15];
  splitNMEA(rmc, tokens);

  String fixStatus = tokens[2];

  if (fixStatus == "A") {
    if (!gpsFixed) {
      Serial.println("✅ FIX получен!");
    }
    gpsFixed = true;
  } else {
    if (gpsFixed) {
      Serial.println("❌ Потеря FIX.");
    }
    gpsFixed = false;
  }
}

void parseGNGGA(String gga) {
  if (!gpsFixed) return;

  String tokens[15];
  splitNMEA(gga, tokens);

  double lat = nmeaToDecimal(tokens[2]);
  String latDir = tokens[3];
  double lon = nmeaToDecimal(tokens[4]);
  String lonDir = tokens[5];
  int satellites = tokens[7].toInt();
  double altitude = tokens[9].toFloat();

  Serial.println("📍 Координаты:");
  Serial.print("  Широта: "); Serial.print(lat); Serial.print(" "); Serial.println(latDir);
  Serial.print("  Долгота: "); Serial.print(lon); Serial.print(" "); Serial.println(lonDir);
  Serial.print("  Спутников: "); Serial.println(satellites);
  Serial.print("  Высота: "); Serial.print(altitude); Serial.println(" м");
  Serial.println("-----------------------------");
}

double nmeaToDecimal(String nmea) {
  if (nmea.length() < 6) return 0.0;
  double raw = nmea.toDouble();
  int degrees = int(raw / 100);
  double minutes = raw - (degrees * 100);
  return degrees + (minutes / 60.0);
}

void splitNMEA(String line, String tokens[]) {
  int idx = 0;
  int last = 0;

  for (int i = 0; i < line.length(); i++) {
    if (line.charAt(i) == ',' || line.charAt(i) == '*') {
      tokens[idx++] = line.substring(last, i);
      last = i + 1;
    }
  }
}
