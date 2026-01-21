#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>

// ========== CONFIGURATION WIFI ==========
const char *ssid = "vivo flo";
const char *password = "fuckyoubitch";

// ========== CONFIGURATION MQTT ==========
const char *mqtt_server = "10.149.212.160";
const int mqtt_port = 8883;
const char *mqtt_user = "esp32";
const char *mqtt_password = "plante2026";
const char *mqtt_topic = "plante/humidite";

// ========== CERTIFICAT CA ==========
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

// ========== OBJETS ==========
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n========================================");
  Serial.println("  ESP32 IoT avec MQTTS");
  Serial.println("========================================\n");

  // Connexion WiFi
  connectWiFi();

  // Synchronisation NTP
  syncNTP();

  // Configuration SSL
  setupSSL();

  // Configuration MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);

  // Connexion MQTT
  reconnectMQTT();

  Serial.println("\n========================================");
  Serial.println("  Systeme pret !");
  Serial.println("========================================\n");
}

// ========== CONNEXION WIFI ==========
void connectWiFi() {
  Serial.print("📡 Connexion WiFi a: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi connecte !");
    Serial.print("   SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("   Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("   IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("   Gateway: ");
    Serial.println(WiFi.gatewayIP());
  } else {
    Serial.println("\n❌ Echec connexion WiFi !");
    Serial.println("   Verifiez SSID et mot de passe");
    while (1) {
      delay(1000);
    }
  }
}

// ========== SYNCHRONISATION NTP ==========
void syncNTP() {
  Serial.println("\n⏰ Synchronisation NTP...");

  // Configure NTP (GMT+1 pour la France)
  configTime(3600, 0, "pool.ntp.org", "time.nist.gov");

  time_t now = 0;
  struct tm timeinfo;
  int retry = 0;

  while (time(&now) && now < 1000000000 && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (now > 1000000000) {
    Serial.println("\n✅ Heure synchronisee !");
    localtime_r(&now, &timeinfo);
    Serial.print("   Date: ");
    Serial.print(timeinfo.tm_mday);
    Serial.print("/");
    Serial.print(timeinfo.tm_mon + 1);
    Serial.print("/");
    Serial.print(timeinfo.tm_year + 1900);
    Serial.print(" ");
    Serial.print(timeinfo.tm_hour);
    Serial.print(":");
    Serial.print(timeinfo.tm_min);
    Serial.print(":");
    Serial.println(timeinfo.tm_sec);
  } else {
    Serial.println("\n⚠️  Echec NTP (continuer quand meme)");
  }
}

// ========== CONFIGURATION SSL ==========
void setupSSL() {
  Serial.println("\n🔐 Configuration SSL...");

  // Certificat CA pour verifier le serveur
  wifiClient.setCACert(ca_cert);

  Serial.println("✅ Certificat CA charge !");
  Serial.println("   Mode: TLS avec verification serveur");
}

// ========== CONNEXION MQTT ==========
void reconnectMQTT() {
  int attempts = 0;
  while (!client.connected() && attempts < 5) {
    Serial.print("\n🔌 Connexion MQTTS a ");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.print(mqtt_port);
    Serial.println("...");

    if (client.connect("ESP32Plante", mqtt_user, mqtt_password)) {
      Serial.println("✅ MQTTS connecte !");

      // Souscription au topic
      if (client.subscribe(mqtt_topic)) {
        Serial.print("   Abonne a: ");
        Serial.println(mqtt_topic);
      }
    } else {
      Serial.print("❌ Echec MQTTS. Code: ");
      Serial.println(client.state());

      switch (client.state()) {
      case -4:
        Serial.println("   -> Timeout");
        break;
      case -3:
        Serial.println("   -> Connexion perdue");
        break;
      case -2:
        Serial.println("   -> Echec TLS/TCP");
        break;
      case -1:
        Serial.println("   -> Deconnecte");
        break;
      case 1:
        Serial.println("   -> Mauvais protocole");
        break;
      case 2:
        Serial.println("   -> ID client rejete");
        break;
      case 3:
        Serial.println("   -> Serveur indisponible");
        break;
      case 4:
        Serial.println("   -> Mauvais credentials");
        break;
      case 5:
        Serial.println("   -> Non autorise");
        break;
      }

      attempts++;
      if (attempts < 5) {
        Serial.println("   Nouvelle tentative dans 3s...");
        delay(3000);
      }
    }
  }

  if (!client.connected()) {
    Serial.println("\n❌ Impossible de se connecter au broker");
    Serial.println("   Verifiez la config et les certificats");
  }
}

// ========== CALLBACK MQTT ==========
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("📨 Message [");
  Serial.print(topic);
  Serial.print("]: ");

  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// ========== LOOP ==========
void loop() {
  // Reconnexion si necessaire
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Publication toutes les 10 secondes
  static unsigned long lastMsg = 0;
  unsigned long now = millis();

  if (now - lastMsg > 10000) {
    lastMsg = now;

    // Simuler une mesure d'humidite
    int humidity = random(30, 80);
    String message = String(humidity);

    Serial.print("📤 Publication: ");
    Serial.print(message);

    if (client.publish(mqtt_topic, message.c_str())) {
      Serial.println(" ✅");
    } else {
      Serial.println(" ❌");
    }
  }
}
