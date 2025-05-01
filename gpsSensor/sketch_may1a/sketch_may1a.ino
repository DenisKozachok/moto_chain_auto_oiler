#include <HardwareSerial.h>
#include <math.h>

HardwareSerial gpsSerial(1);
const int RXPin = 16;
const int TXPin = 17;

String gpsLine = "";
bool gpsFixed = false;

const int ledPin = 2;
unsigned long lastBlink = 0;
const unsigned long blinkInterval = 10000;

// Last known coordinates and timestamp
double lastLat = 0.0;
double lastLon = 0.0;
unsigned long lastFixTime = 0;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, RXPin, TXPin);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println("📡 GPS parser with distance and speed started...");
}

void loop() {
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

  // LED blink logic if no fix
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
      Serial.println("✅ GPS FIX acquired!");
    }
    gpsFixed = true;
  } else {
    if (gpsFixed) {
      Serial.println("❌ GPS FIX lost.");
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

  if (latDir == "S") lat = -lat;
  if (lonDir == "W") lon = -lon;

  Serial.println("📍 Coordinates:");
  Serial.print("  Latitude: "); Serial.println(lat, 6);
  Serial.print("  Longitude: "); Serial.println(lon, 6);
  Serial.print("  Satellites: "); Serial.println(satellites);
  Serial.print("  Altitude: "); Serial.print(altitude); Serial.println(" m");

  // Calculate distance and speed
  if (lastLat != 0.0 && lastLon != 0.0) {
    double distance = haversine(lastLat, lastLon, lat, lon); // meters
    unsigned long now = millis();
    double hoursPassed = (now - lastFixTime) / 3600000.0; // convert ms to hours
    double speedKph = distance / 1000.0 / hoursPassed;

    Serial.print("📏 Distance: "); Serial.print(distance); Serial.println(" m");
    Serial.print("🚀 Speed: "); Serial.print(speedKph); Serial.println(" km/h");
    Serial.println("-----------------------------");
  }

  // Save current location and time
  lastLat = lat;
  lastLon = lon;
  lastFixTime = millis();
}

// Convert NMEA format to decimal
double nmeaToDecimal(String nmea) {
  if (nmea.length() < 6) return 0.0;
  double raw = nmea.toDouble();
  int degrees = int(raw / 100);
  double minutes = raw - (degrees * 100);
  return degrees + (minutes / 60.0);
}

// Split a NMEA sentence by commas
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

// Haversine formula to calculate distance in meters
double haversine(double lat1, double lon1, double lat2, double lon2) {
  const double R = 6371000; // Earth radius in meters
  double dLat = radians(lat2 - lat1);
  double dLon = radians(lon2 - lon1);
  double a = sin(dLat / 2) * sin(dLat / 2) +
             cos(radians(lat1)) * cos(radians(lat2)) *
             sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c;
}
