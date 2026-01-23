#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// ================== 1. SECURITE XTEA (128 bits) ==================
const uint32_t XTEA_KEY[4] = {0xA1B2C3D4, 0xE5F67890, 0x1A2B3C4D, 0x5E6F7809};

void xtea_enc(uint32_t v[2]) {
  uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
  for (int i = 0; i < 32; i++) {
    v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + XTEA_KEY[sum & 3]);
    sum += delta;
    v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + XTEA_KEY[(sum >> 11) & 3]);
  }
  v[0] = v0;
  v[1] = v1;
}

String encryptMessage(String plain) {
  while (plain.length() % 8 != 0)
    plain += " "; // Padding
  String hexOut = "";
  for (int i = 0; i < plain.length(); i += 8) {
    uint32_t b[2];
    b[0] = (uint8_t)plain[i] | ((uint8_t)plain[i + 1] << 8) |
           ((uint8_t)plain[i + 2] << 16) | ((uint8_t)plain[i + 3] << 24);
    b[1] = (uint8_t)plain[i + 4] | ((uint8_t)plain[i + 5] << 8) |
           ((uint8_t)plain[i + 6] << 16) | ((uint8_t)plain[i + 7] << 24);
    xtea_enc(b);
    char buf[17];
    snprintf(buf, sizeof(buf), "%08X%08X", b[0], b[1]);
    hexOut += String(buf);
  }
  return hexOut;
}

// ================== 2. CONFIGURATION & BUFFER ==================
const int BUFFER_SIZE = 30;
struct Record {
  char payload[64];
  bool ready;
};
Record history[BUFFER_SIZE];
int head = 0;
int tail = 0;
int bufferCount = 0;

const int PIN_LIGHT = A0;
const int PIN_SOIL = A1;
Adafruit_BME280 bme;
#define LORA Serial1

// --- FONCTIONS DE FILTRAGE ET MOYENNE ---

float getFilteredTemp() {
  float sum = 0;
  int validCount = 0;
  for (int i = 0; i < 5; i++) {
    float t = bme.readTemperature();
    if (!isnan(t) && t > -20.0 && t < 60.0) {
      sum += t;
      validCount++;
    }
    delay(50);
  }
  return (validCount > 0) ? (sum / validCount) : -99.0;
}

int getFilteredSoil() {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(PIN_SOIL);
    delay(20);
  }
  return (int)(sum / 10);
}

// --- COMMUNICATION LORA ---

void loraFlush() {
  while (LORA.available())
    LORA.read();
}

bool sendWithAck(String clearPayload) {
  String encrypted = encryptMessage(clearPayload);
  loraFlush();
  LORA.print("AT+TEST=TXLRPKT,\"" + encrypted + "\"\r\n");
  delay(600);
  loraFlush();
  LORA.print("AT+TEST=RXLRPKT\r\n");

  unsigned long start = millis();
  String rxBuf = "";
  while (millis() - start < 4000) {
    while (LORA.available()) {
      char c = (char)LORA.read();
      if (c == '\n') {
        if (rxBuf.indexOf("+TEST: RX") != -1)
          return true;
        rxBuf = "";
      } else if (c != '\r')
        rxBuf += c;
    }
  }
  return false;
}

// ================== SETUP & LOOP ==================

void setup() {
  Serial.begin(9600);
  LORA.begin(9600);
  if (!bme.begin(0x76))
    bme.begin(0x77);

  delay(1000);
  LORA.println("AT+MODE=TEST");
  delay(500);
  LORA.println("AT+TEST=RFCFG,868,SF7,125,12,15,14,ON,OFF,OFF");
  delay(500);
  Serial.println("=== SENDER FILTRE (SANS ID) PRET ===");
}

const unsigned long INTERVAL_NORMAL = 15000;
const unsigned long INTERVAL_RETRY = 5000;
unsigned long previousMillis = 0;

void loop() {
  unsigned long currentMillis = millis();
  unsigned long interval =
      (bufferCount == 0) ? INTERVAL_NORMAL : INTERVAL_RETRY;

  // Non-blocking wait
  if (currentMillis - previousMillis < interval) {
    return;
  }
  previousMillis = currentMillis;

  // 1. Capteurs avec filtrage
  int soil = getFilteredSoil();
  int light = analogRead(PIN_LIGHT);
  float t = getFilteredTemp();
  float h = bme.readHumidity();

  if (t == -99.0) {
    Serial.println("ERREUR: Capteur BME280 HS.");
    return; // Retry next cycle
  }

  // Format sans ID
  String data = "S=" + String(soil) + ";L=" + String(light) +
                ";T=" + String(t, 1) + ";H=" + String(h, 1);

  // 2. Bufferisation
  if (bufferCount < BUFFER_SIZE) {
    data.toCharArray(history[head].payload, 64);
    history[head].ready = true;
    head = (head + 1) % BUFFER_SIZE;
    bufferCount++;
    Serial.println("Mesure stockee. Buffer: " + String(bufferCount));
  }

  // 3. Tentative d'envoi
  int sentInThisTurn = 0;
  while (bufferCount > 0 && sentInThisTurn < 5) {
    Serial.print("Envoi mesure du buffer... ");
    if (sendWithAck(String(history[tail].payload))) {
      Serial.println("OK (ACK recu)");
      history[tail].ready = false;
      tail = (tail + 1) % BUFFER_SIZE;
      bufferCount--;
      sentInThisTurn++;
      delay(200); // Petit delai pour la radio (acceptable ici)
    } else {
      Serial.println("ECHEC (Gateway absente)");
      break;
    }
  }
}
