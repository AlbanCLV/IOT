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

// ========== CERTIFICAT CLIENT ESP32 ==========
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

// ========== CLE PRIVEE CLIENT ESP32 ==========
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

// ========== CONFIGURATION SSL avec mTLS ==========
void setupSSL() {
  Serial.println("\n🔐 Configuration SSL avec mTLS...");

  // 1. Certificat CA (pour vérifier le serveur)
  wifiClient.setCACert(ca_cert);

  // 2. Certificat Client (pour s'identifier au serveur)
  wifiClient.setCertificate(client_cert);

  // 3. Clé Privée Client (pour prouver son identité)
  wifiClient.setPrivateKey(client_key);

  Serial.println("✅ Certificat CA charge !");
  Serial.println("✅ Certificat client charge !");
  Serial.println("   Mode: mTLS (Authentification Mutuelle)");
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

    // Connexion avec certificat client uniquement (pas de username/password)
    if (client.connect("ESP32Plante")) {
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
