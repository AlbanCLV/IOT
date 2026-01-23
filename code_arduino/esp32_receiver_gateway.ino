#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>

// ================== CONFIGURATION WIFI ==================
const char *ssid = "vivo flo";
const char *password = "fuckyoubitch";

// ================== CONFIGURATION MQTT ==================
const char *mqtt_server = "10.149.212.160";
const int mqtt_port = 8883;
const char *mqtt_topic =
    "maison/plante/data/raw"; // Topic pour data brute (hex)

// ================== CERTIFICATS TLS ==================
const char *ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDmTCCAoGgAwIBAgIUTZhGA9GSfNwIGN6XiETA14n3wl0wDQYJKoZIhvcNAQEL
BQAwVDELMAkGA1UEBhMCRlIxDzANBgNVBAgMBkZyYW5jZTENMAsGA1UEBwwETHlv
bjEPMA0GA1UECgwGTWFpc29uMRQwEgYDVQQDDAtNb3NxdWl0dG9DQTAeFw0yNjAx
MjExNDA2MTJaFw0zNjAxMTkxNDA2MTJaMFQxCzAJBgNVBAYTAkZSMQ8wDQYDVQQI
DAZGcmFuY2UxDTALBgNVBAcMBEx5b24xDzANBgNVBAoMBk1haXNvbjEUMBIGA1UE
AwwLTW9zcXVpdHRvQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDf
5Sl0g25JLKeFyAbZQDyO7we/98sS2HtvC4fP8NNdKNyCcrEB3PmZqhTVtFCwXEwN
X2Q73TJMDH1MkL4t5V3SZQLYcqDoYdgZmQvwppDIwLwGwqO9G8UDaV8F7sg8HBK/
rqcNpQz0ftAToUQ3xP/G36PT6Uw1CwJWZKgm+UZWwyO5NqysnatT9DzdAI46ezfN
HN2v3MCW+CsDlI+uoVegAAIhdxPWNGsGUP5uj56uhYWqQmfeqL1CcdJSe/Whzudv
2tfBEoC3NXcUJkPxw4Q5EAwwHni0uMkTSa2wdrjXHd3crdoM+Vb7cPehNRVqbUE6
fjN0YDhxyEd1VV8vc9nnAgMBAAGjYzBhMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0P
AQH/BAQDAgEGMB0GA1UdDgQWBBRppD+2WspPE6zSzk72YeFLs42iiDAfBgNVHSME
GDAWgBRppD+2WspPE6zSzk72YeFLs42iiDANBgkqhkiG9w0BAQsFAAOCAQEAfFrM
S/0cfIypOEKL0yWNoF6zNQdMOPvuDhyH/pQQfT91kHDy4UUbY9WxIsUrV/TqWBNs
UsbfmObpzXEVFoG5XVBp9LrUdBwedEICIP5vhTPZvMvJzFICkWGbf754ZVWkSOKM
U94yh9TID9j+vvyEK5stO020bGSNl9/v/brhq7Ma+HCM4nHw9wSYSXz2hpX/Cm+x
Lhspc7xDRtSzB7xXeYFWyZu0eP/oRmwoDNr6cqNnJpsLd4bhzyfrwoB8iriZA7Wa
1W9nNDmA/+EwGKJgT6a6sTleFHiJjkEYlbRmEw41C7tEEp545uB7o77E+cFH8VEa
IfK2ylrA+TGfuRR/Gg==
-----END CERTIFICATE-----
)EOF";

const char *client_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDeDCCAmCgAwIBAgIUNwaWY2Wq5LsSCJPKSS59s82lfekwDQYJKoZIhvcNAQEL
BQAwVDELMAkGA1UEBhMCRlIxDzANBgNVBAgMBkZyYW5jZTENMAsGA1UEBwwETHlv
bjEPMA0GA1UECgwGTWFpc29uMRQwEgYDVQQDDAtNb3NxdWl0dG9DQTAeFw0yNjAx
MjExNTM5NTNaFw0zNjAxMTkxNTM5NTNaMFQxCzAJBgNVBAYTAkZSMQ8wDQYDVQQI
DAZGcmFuY2UxDTALBgNVBAcMBEx5b24xDzANBgNVBAoMBk1haXNvbjEUMBIGA1UE
AwwLRVNQMzJQbGFudGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDE
JGuKAwxMBh+S651kxTgT3FTomaeHhi827c7CEXhhvKIyJjOZFNjS8u3tkMGAtDeN
aTzZKLM4ldWVnuWCxNRV7Pdp4GoSxWN1fV7zX+vm3+u4WQKrF5RGgpR8nzZvna25
18hLQO1pChhXpX6yUKtuFcUGf6V6lFn74a75yotrdRZY0MAwDyjvsh2Mzcz+BPcY
WLm949iCpvmnBJrFmOxly1c0Bv6ibXAqvspVybo4ccJrqjMv3s5gESCppvCYULeP
L+3UOQqYJfJTuIfmTLT3S4Iqz+VvismH7VFcY8Vb6WrfZ1DnKVBZTML1fjHgbDzf
f8+9dSZi9dAPgZjY2sFZAgMBAAGjQjBAMB0GA1UdDgQWBBTyWDe3eA1sbclXhrIr
gPCcU1iYqTAfBgNVHSMEGDAWgBRppD+2WspPE6zSzk72YeFLs42iiDANBgkqhkiG
9w0BAQsFAAOCAQEAUnrVE/rKo6tLlwnq6imQdhJuXtPMk2Fc65qSnHiq754npLca
ICQJdlViP3P752utpmdHR9fY0TpVnNn3fy76hFASaS88l8Hv9o3/MthSNdQr2AHR
vHo9DfcBnnIzDBrPAQAR3WNziFTftucwsqgGK/uNvK+U+gqaFeIedJ3sufyCIde9
Ok81j/bOws1ecLboaRRHXvidtTeMjUXdan95UbgyTzFkZu2BFchab1malzrIPRUx
OzMPnUvPKop4LydOqGpbKGIcN5NyPSJ7KRhqYri4XBZoj5v7bTFyY7iwQ01o8Q7j
tOMlY+fHtPuMkyp/WbO4pK9Qv1lAfgLS8r9JxA==
-----END CERTIFICATE-----
)EOF";

const char *client_key = R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDEJGuKAwxMBh+S
651kxTgT3FTomaeHhi827c7CEXhhvKIyJjOZFNjS8u3tkMGAtDeNaTzZKLM4ldWV
nuWCxNRV7Pdp4GoSxWN1fV7zX+vm3+u4WQKrF5RGgpR8nzZvna2518hLQO1pChhX
pX6yUKtuFcUGf6V6lFn74a75yotrdRZY0MAwDyjvsh2Mzcz+BPcYWLm949iCpvmn
BJrFmOxly1c0Bv6ibXAqvspVybo4ccJrqjMv3s5gESCppvCYULePL+3UOQqYJfJT
uIfmTLT3S4Iqz+VvismH7VFcY8Vb6WrfZ1DnKVBZTML1fjHgbDzff8+9dSZi9dAP
gZjY2sFZAgMBAAECggEAK1MQHfAGa1R+7Oj2Xyfg9yIX7zjhdGIDU9K7dMp/GLed
7A3cH9gm/JGEVJFA7Ljh/G5ebmk7DSirgN0UPQpVDJXHIK/dPkAzc+Q5cPCjHuAf
JR0kJb6Tle+J9O0GDSl7Ei3DpDu3NRhxwApiLej7tNlCJ+lSF/1k0upKyAohOZ7E
OOola29/Atdbc330TnYpNd01VYlCnIjCOBqZWKZN73EcltgYC9cDk09IyNq64wUF
MIQGt23VISXBUTiSW4Z0R7EfnaGjlk/2g2YzYzqP7JfQOw5/eVLASyi2OvoOO9g/
FAm7lbAmZgva0EOpG7yHZxOih7ok1rlcvhdy467YcQKBgQDpkLHD1lfJglxgMSLY
ZMxnfzJQbG4A70ZvCUPuuQ3Xjb3ZuAhUWxt+6aCW5vcdnQaPJa4JLNtO+imi/0Ke
qjSWu62oTuBe9LFlUx8HBp+a63MQl2UewiPY3fiGm9z/2HlKf6M1mizh7lvbzaw4
Y5dtToQXjP2I3+ehysMRmgygjwKBgQDW+4FpVFLz3UVb1L76UigVt3QqmJOevdS3
BjfE0gygl0YUcIXvdhgo9GM9Q1ParxXjB5KjDcGPZyBkJbmjIqdX9rYidPvXSNkg
E9PiLwJtLZeQA1jDkWvc9wroicU2KkO44TOqskmIsX9ahtkYROtkMYEaPj/LDTzy
pXB8uIOjlwKBgC3z1+Wt4Cu3SDFsuWxVuJmMPvvvuVzZtsPV0NXA9uY37uhM+uaN
3kUD9iJ6Ypgi8s1rHmiTuLo7IcH98r+AwUP+vNXmXuKPxe8ngW1CclhqhoL3Jl/8
1kyur1zcgYicsJBluFitcrMo0E3Yk0+s2HgbAIYZqTAqzUnZuLNX2bAlAoGBAMno
NnelQa9VfqGrop3Y8VrMDKdA20v4f8JRbalFmaRtzty9fZ0T7cbxGA9en2/Ahq7N
2vOWpNa5BVvpbnr/velIhXDBLBHakkmlNZe1jPLbIgCYlBJBDuvdfXo+gmDHuwj1
QSAtCBEAm1p7UYOQnv9f2x+5nRJoYXiG1qncZY4LAoGBANCO+0/SmY86e1jy+HDM
4TbPigCens+zeYxToqzbGYAmcPTuFPJksxU9ARN/UeZJYD8DG8tQDgfD5/RPaAp8
x2/BkJnN1GW0feDcFmF7vyXdbRE7/V/xz9a4einDyCcxrBvEbOX8P3Qq0fZFKsnk
/yCnsP5ZOdTIjwv4c/mrKfSy
-----END PRIVATE KEY-----
)EOF";

// ================== BUFFER MQTT ==================
#define MAX_MQTT_BUFFER 20
struct SavedMessage {
  String payload;
  bool ready;
};
SavedMessage mqttBuffer[MAX_MQTT_BUFFER];
int mqttHead = 0;
int mqttTail = 0;
int mqttCount = 0;

// ================== OBJETS ==================
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);
HardwareSerial LoRaSerial(2); // UART2 sur ESP32

String lineBuf = "";
unsigned long lastReconnect = 0;
unsigned long messageCount = 0;

// ================== FONCTIONS UTILITAIRES ==================
void printSeparator() {
  Serial.println("========================================");
}

void printHeader(String title) {
  printSeparator();
  Serial.print("  ");
  Serial.println(title);
  printSeparator();
}

void addToMqttBuffer(String data) {
  if (mqttCount < MAX_MQTT_BUFFER) {
    mqttBuffer[mqttHead].payload = data;
    mqttBuffer[mqttHead].ready = true;
    mqttHead = (mqttHead + 1) % MAX_MQTT_BUFFER;
    mqttCount++;
    Serial.print("[BUFFER] Message #");
    Serial.print(mqttCount);
    Serial.println(" ajoute (WiFi/MQTT offline)");
  } else {
    Serial.println("[BUFFER] PLEIN ! Message perdu !");
  }
}

// ================== WIFI ==================
void connectWiFi() {
  Serial.println("\n[WiFi] Demarrage connexion...");
  Serial.print("[WiFi] SSID: ");
  Serial.println(ssid);
  Serial.print("[WiFi] Mode: ");
  Serial.println("Station (STA)");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  Serial.print("[WiFi] Tentative de connexion");

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;

    if (attempts % 10 == 0) {
      Serial.println();
      Serial.print("[WiFi] ");
      Serial.print(attempts);
      Serial.print(" tentatives...");
    }
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[WiFi] *** CONNECTE ***");
    Serial.print("[WiFi] IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WiFi] Gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("[WiFi] Subnet: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("[WiFi] DNS: ");
    Serial.println(WiFi.dnsIP());
    Serial.print("[WiFi] RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("[WiFi] MAC: ");
    Serial.println(WiFi.macAddress());
  } else {
    Serial.println("[WiFi] *** ECHEC ***");
    Serial.print("[WiFi] Status code: ");
    Serial.println(WiFi.status());
  }
}

// ================== NTP ==================
void syncNTP() {
  Serial.println("\n[NTP] Demarrage synchronisation...");
  Serial.println("[NTP] Serveurs: pool.ntp.org, time.nist.gov");
  Serial.println("[NTP] Timezone: GMT+1 (France)");

  configTime(3600, 0, "pool.ntp.org", "time.nist.gov");

  time_t now = 0;
  int retry = 0;
  Serial.print("[NTP] Attente reponse");

  while (time(&now) && now < 1000000000 && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;

    if (retry % 10 == 0) {
      Serial.println();
      Serial.print("[NTP] ");
      Serial.print(retry * 500);
      Serial.print("ms...");
    }
  }

  Serial.println();

  if (now > 1000000000) {
    Serial.println("[NTP] *** SYNCHRONISE ***");
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    Serial.print("[NTP] Date: ");
    Serial.print(timeinfo.tm_mday);
    Serial.print("/");
    Serial.print(timeinfo.tm_mon + 1);
    Serial.print("/");
    Serial.println(timeinfo.tm_year + 1900);
    Serial.print("[NTP] Heure: ");
    Serial.print(timeinfo.tm_hour);
    Serial.print(":");
    if (timeinfo.tm_min < 10)
      Serial.print("0");
    Serial.print(timeinfo.tm_min);
    Serial.print(":");
    if (timeinfo.tm_sec < 10)
      Serial.print("0");
    Serial.println(timeinfo.tm_sec);
    Serial.print("[NTP] Timestamp: ");
    Serial.println(now);
  } else {
    Serial.println("[NTP] *** ECHEC ***");
    Serial.println("[NTP] Continuer sans synchro (peut affecter TLS)");
  }
}

// ================== SSL/mTLS ==================
void setupSSL() {
  Serial.println("\n[TLS] Configuration mTLS...");
  Serial.println("[TLS] Mode: Authentification mutuelle");

  Serial.print("[TLS] Chargement CA...");
  wifiClient.setCACert(ca_cert);
  Serial.println(" OK");

  Serial.print("[TLS] Chargement certificat client...");
  wifiClient.setCertificate(client_cert);
  Serial.println(" OK");

  Serial.print("[TLS] Chargement cle privee...");
  wifiClient.setPrivateKey(client_key);
  Serial.println(" OK");

  Serial.println("[TLS] *** mTLS PRET ***");
}

// ================== MQTT ==================
void reconnectMQTT() {
  if (!client.connected()) {
    Serial.println("\n[MQTT] Tentative de connexion...");
    Serial.print("[MQTT] Broker: ");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.println(mqtt_port);
    Serial.println("[MQTT] Protocol: MQTTS (TLS 1.2)");
    Serial.println("[MQTT] Auth: Certificat client");
    Serial.print("[MQTT] Client ID: ESP32_LoRa_Gateway...");

    if (client.connect("ESP32_LoRa_Gateway")) {
      Serial.println(" OK");
      Serial.println("[MQTT] *** CONNECTE ***");
      Serial.print("[MQTT] Topic publication: ");
      Serial.println(mqtt_topic);

      // Vider buffer
      if (mqttCount > 0) {
        Serial.print("[MQTT] Buffer a vider: ");
        Serial.print(mqttCount);
        Serial.println(" messages");
      }
    } else {
      Serial.println(" ECHEC");
      Serial.print("[MQTT] Code erreur: ");
      int state = client.state();
      Serial.println(state);

      switch (state) {
      case -4:
        Serial.println("[MQTT] Timeout connexion");
        break;
      case -3:
        Serial.println("[MQTT] Connexion perdue");
        break;
      case -2:
        Serial.println("[MQTT] Echec TLS/TCP");
        break;
      case -1:
        Serial.println("[MQTT] Deconnecte");
        break;
      case 1:
        Serial.println("[MQTT] Mauvais protocole");
        break;
      case 2:
        Serial.println("[MQTT] ID client rejete");
        break;
      case 3:
        Serial.println("[MQTT] Serveur indisponible");
        break;
      case 4:
        Serial.println("[MQTT] Mauvais credentials");
        break;
      case 5:
        Serial.println("[MQTT] Non autorise");
        break;
      default:
        Serial.println("[MQTT] Erreur inconnue");
      }
    }
  }
}

// ================== LORA ==================
void setupLoRa() {
  Serial.println("\n[LoRa] Configuration module...");
  Serial.println("[LoRa] Broches: RX=GPIO16, TX=GPIO17");
  Serial.println("[LoRa] Baud: 9600");

  Serial.print("[LoRa] Mode TEST...");
  LoRaSerial.println("AT+MODE=TEST");
  delay(500);
  Serial.println(" OK");

  Serial.print("[LoRa] Config RF (868 MHz, SF7)...");
  LoRaSerial.println("AT+TEST=RFCFG,868,SF7,125,12,15,14,ON,OFF,OFF");
  delay(500);
  Serial.println(" OK");

  Serial.print("[LoRa] Mode RX...");
  LoRaSerial.println("AT+TEST=RXLRPKT");
  Serial.println(" OK");

  Serial.println("[LoRa] *** PRET A RECEVOIR ***");
}

void processLoRaData(String hex) {
  messageCount++;

  Serial.println("\n----------------------------------------");
  Serial.print("[LoRa] MESSAGE #");
  Serial.println(messageCount);
  Serial.println("----------------------------------------");
  Serial.print("[LoRa] Data HEX brute: ");
  Serial.println(hex);
  Serial.print("[LoRa] Longueur: ");
  Serial.print(hex.length());
  Serial.println(" caracteres");
  Serial.print("[LoRa] Timestamp: ");
  Serial.print(millis() / 1000);
  Serial.println(" secondes");

  // Envoyer ACK
  Serial.println("[LoRa] Envoi ACK...");
  LoRaSerial.print("AT+TEST=TXLRPKT,\"41434B\"\r\n");
  delay(400);
  while (LoRaSerial.available())
    LoRaSerial.read();
  LoRaSerial.print("AT+TEST=RXLRPKT\r\n");
  Serial.println("[LoRa] ACK envoye (HEX: 41434B = 'ACK')");

  // Creer payload JSON avec data brute
  String json = "{";
  json += "\"hex\":\"" + hex + "\",";
  json += "\"timestamp\":" + String(millis() / 1000) + ",";
  json += "\"rssi\":" + String(WiFi.RSSI());
  json += "}";

  Serial.println("[LoRa] Payload JSON:");
  Serial.println(json);

  // Envoyer MQTT ou buffer
  if (client.connected()) {
    Serial.print("[MQTT] Publication... ");

    if (client.publish(mqtt_topic, json.c_str())) {
      Serial.println("OK");
      Serial.print("[MQTT] Taille: ");
      Serial.print(json.length());
      Serial.println(" bytes");
    } else {
      Serial.println("ECHEC");
      Serial.println("[MQTT] Message mis en buffer");
      addToMqttBuffer(json);
    }
  } else {
    Serial.println("[MQTT] Non connecte");
    addToMqttBuffer(json);
  }
}

// ================== SETUP ==================
void setup() {
  Serial.begin(9600); // 9600 bauds comme demande
  delay(3000);        // Laisser le temps d'ouvrir le moniteur

  printHeader("ESP32 LoRa Gateway + MQTTS");
  Serial.println("[SYSTEM] Demarrage...");
  Serial.print("[SYSTEM] Build: ");
  Serial.println(__DATE__);
  Serial.print("[SYSTEM] Heure: ");
  Serial.println(__TIME__);

  // Init LoRa sur GPIO 16 (RX2), GPIO 17 (TX2)
  LoRaSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("[SYSTEM] UART LoRa initialise");

  // WiFi
  connectWiFi();

  // NTP (seulement si WiFi OK)
  if (WiFi.status() == WL_CONNECTED) {
    syncNTP();
  } else {
    Serial.println("[NTP] Skip (pas de WiFi)");
  }

  // SSL/mTLS
  setupSSL();

  // MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setKeepAlive(60);
  Serial.println("[MQTT] Serveur configure");

  // LoRa
  setupLoRa();

  printHeader("Gateway PRETE !");
  Serial.println("[SYSTEM] En attente de messages LoRa...\n");
}

// ================== LOOP ==================
void loop() {
  static unsigned long lastStatusPrint = 0;

  // Afficher status toutes les 30 secondes
  if (millis() - lastStatusPrint > 30000) {
    lastStatusPrint = millis();
    Serial.println("\n--- STATUS ---");
    Serial.print("[WiFi] ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "Connecte" : "Deconnecte");
    Serial.print("[MQTT] ");
    Serial.println(client.connected() ? "Connecte" : "Deconnecte");
    Serial.print("[Buffer] ");
    Serial.print(mqttCount);
    Serial.println(" messages");
    Serial.print("[Messages] ");
    Serial.println(messageCount);
    Serial.println("--------------\n");
  }

  // --- GESTION RESEAU ---
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastReconnect > 10000) {
      lastReconnect = millis();
      Serial.println("\n[WiFi] Connexion perdue !");
      connectWiFi();
    }
  } else if (!client.connected()) {
    if (millis() - lastReconnect > 5000) {
      lastReconnect = millis();
      reconnectMQTT();
    }
  } else {
    client.loop();

    // Vider buffer MQTT
    if (mqttCount > 0) {
      Serial.print("[MQTT] Vidage buffer (");
      Serial.print(mqttCount);
      Serial.print(" restants)... ");

      if (client.publish(mqtt_topic, mqttBuffer[mqttTail].payload.c_str())) {
        Serial.println("OK");
        mqttBuffer[mqttTail].ready = false;
        mqttTail = (mqttTail + 1) % MAX_MQTT_BUFFER;
        mqttCount--;
      } else {
        Serial.println("ECHEC");
      }

      delay(200);
    }
  }

  // --- LECTURE LORA ---
  while (LoRaSerial.available()) {
    char c = (char)LoRaSerial.read();

    if (c == '\n') {
      String line = lineBuf;
      lineBuf = "";
      line.trim();

      if (line.indexOf("+TEST: RX") >= 0) {
        int q1 = line.indexOf('\"');
        int q2 = line.lastIndexOf('\"');

        if (q1 >= 0 && q2 > q1) {
          String hex = line.substring(q1 + 1, q2);
          processLoRaData(hex);
        }
      }
    } else if (c != '\r') {
      lineBuf += c;
    }
  }
}
