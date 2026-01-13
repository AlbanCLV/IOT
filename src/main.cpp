#include <Arduino.h>

const int PIN_NTC = A0;
const int LED_VERTE = 2;
const int LED_ROUGE = 3;

// Paramètres du capteur NTC (Standard Wokwi)
const float BETA = 3950; 

void setup() {
  Serial.begin(115200);
  pinMode(LED_VERTE, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
  Serial.println("Système de surveillance de température démarré");
}

void loop() {
  // Lecture de la valeur analogique
  int analogValue = analogRead(PIN_NTC);
  
  // Conversion en Celsius (Formule pour NTC)
  float celsius = 1 / (log(1 / (1023.0 / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;

  Serial.print("Température : ");
  Serial.print(celsius);
  Serial.println(" °C");

  // Logique de contrôle des LED
  if (celsius >= 0 && celsius <= 30) {
    digitalWrite(LED_VERTE, HIGH);
    digitalWrite(LED_ROUGE, LOW);
  } 
  else if (celsius > 30) {
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_ROUGE, HIGH);
  } 
  else {
    // Si inférieur à 0, on éteint tout ou on définit un autre état
    digitalWrite(LED_VERTE, LOW);
    digitalWrite(LED_ROUGE, LOW);
  }

  delay(1000); // Mise à jour chaque seconde
}