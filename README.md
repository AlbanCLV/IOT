# EcoGuardian - Un Campus qui vous comprend

![Logo ou Bannière](images/logo.png)

## 📌 Contexte du Projet
**Projet :** « Un campus qui vous comprend »  
**Mandat :** Cellule d’ingénierie opérationnelle  
**Objectif :** Concevoir un système connecté, interopérable et fiable pour un environnement réel.

Le campus CESI lance un programme de modernisation numérique de ses bâtiments. Il ne s’agit plus de tests ou de maquettes isolées : le besoin est concret et les attentes sont élevées.

Notre mission est de concevoir et livrer un système IoT complet, interconnecté, fiable, sécurisé, et capable de rendre un vrai service au quotidien. Ce n'est pas un terrain de jeu technologique, mais un banc d’essai pour une future solution industrialisable.

---

## 🏗️ Architecture du Système

Nous avons conçu une architecture distribuée et résiliente, capable de collecter, transmettre et valoriser des données en temps réel.

### Chaîne de Valeur de la Donnée
1.  **Capteurs (Edge)** : ESP32/Arduino avec capteurs Température, Humidité, Luminosité et humidité dans le Sol.
2.  **Transmission** : Protocole LoRa (Longue Portée) pour la liaison capteur-passerelle.
3.  **Transport (Broker)** : **Mosquitto** avec sécurisation TLS (MQTTS).
4.  **Traitement (Logic)** : **Node-RED** pour le déchiffrement, le filtrage et le routage.
5.  **Stockage (Time-Series)** : **InfluxDB** pour l'historisation.
6.  **Visualisation** : Web App et Grafana.

![Architecture Réseau](images/schema_logique.webp)

---

## 🔐 Sécurité & Confidentialité de Bout en Bout

La sécurité n'est pas une option, c'est une fondation. Nous avons mis en place une stratégie de défense en profondeur.

### 1. Chiffrement Payload (Niveau Applicatif)
La donnée est chiffrée **dès sa création** sur le microcontrôleur.
- **Algorithme** : XTEA (eXtended Tiny Encryption Algorithm) avec clé 128 bits.
- **Principe** : Le capteur chiffre les données brutes avant l'envoi LoRa. La passerelle LoRa transmet les paquets chiffrés sans pouvoir les lire.
- **Déchiffrement** : Seul le cœur du système (Node-RED), qui possède la clé privée, peut déchiffrer et exploiter la donnée.
- **Avantage** : Même si le signal radio est intercepté, la donnée reste inintelligible.

### 2. Transport Sécurisé (MQTTS)
La communication entre la passerelle et le serveur central est encapsulée dans un tunnel chiffré.
- **Protocole** : MQTT over SSL/TLS (Port 8883).
- **Certificats** : Utilisation de certificats X.509 pour authentifier le serveur et chiffrer les échanges.
- **Protection** : Empêche les attaques de type "Man-in-the-Middle" sur le réseau IP du campus.

---

## 🛡️ Résilience & Tolérance aux Pannes

Dans un environnement réel, le réseau n'est jamais garanti. Notre système est conçu pour ne perdre aucune donnée critique.

### 1. Système de Buffer Circulaire (Côté Plante)
Le capteur ne se contente pas d'envoyer et d'oublier ("Fire and Forget"). Il possède une mémoire tampon locale.
- **Fonctionnement** : Les mesures sont stockées dans un buffer circulaire (capacité : 30 mesures).
- **Rupture de lien** : Si la passerelle est injoignable, le capteur continue d'enregistrer localement.
- **Synchronisation** : Dès le retour du réseau, le buffer se vide séquentiellement, garantissant la continuité de l'historique.

### 2. Mécanisme d'Acquittement (ACK)
Chaque transmission est vérifiée.
- **Processus** : Le capteur envoie un paquet chiffré -> La passerelle le reçoit -> La passerelle renvoie un ACK.
- **Retry Logic** : Si aucun ACK n'est reçu sous 4 secondes, le capteur tente une retransmission (jusqu'à 5 essais).
- **Repli** : Après échec, il passe en mode économie d'énergie et réessaiera au prochain cycle, sans supprimer la donnée du buffer.

---

## 🚀 Fonctionnalités Clés "EcoGuardian"

### Surveillance Temps Réel
L'application web offre une vue synthétique et esthétique.
- **Tableau de bord** : Température, Humidité Air/Sol, Luminosité.
- **Score de Bien-être** : Algorithme calculant la santé globale de la plante (0-100%).
- **Plante Virtuelle** : Avatar dynamique qui change d'humeur selon les données.

![Dashboard](images/interface_web_1.png)

### Base de Données Intelligente
- Sélection parmi **9 profils de plantes** (Monstera, Cactus, Orchidée...).
- Seuils d'alerte adaptés automatiquement à chaque espèce.
- **Fiche Détail** : Affichage des besoins spécifiques de la plante sélectionnée.

![Détails Plante](images/interface_web_2.png) 

### Alertes
- **Visuelles** : Les cartes clignotent en cas de danger critique.
- **Discord** : Envoi automatique d'un rapport si la santé passe sous 50%.

---

*« Vous n’êtes pas là pour “brancher des fils”. Vous êtes là pour penser comme des concepteurs de systèmes critiques. »*
