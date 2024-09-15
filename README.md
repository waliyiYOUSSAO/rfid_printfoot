# README - Système de Gestion des Employés avec RFID

## Présentation du Système
Ce système de gestion des employés repose sur une technologie de badges RFID, combinée à un microcontrôleur **ESP8266** et un module horloge pour la gestion du temps. Il permet de :
- Suivre les horaires d'arrivée et de départ des employés.
- Gérer les utilisateurs (ajout, suppression).
- Stocker temporairement les données localement en l'absence de connexion réseau et de les synchroniser avec **Google Sheets** lorsque la connexion est rétablie.
- Gérer les accès via un serveur Web intégré, affichant les informations sur un écran LCD.

### Fonctionnement :
Lorsque l'employé approche son badge RFID du lecteur, le microcontrôleur lit l'identifiant et enregistre les données dans **Google Sheets** via une connexion WiFi. En cas de perte de connexion, les données sont stockées localement.

## Fonctionnalités
- **Ajout d'un utilisateur** : Enregistre un utilisateur et son badge RFID.
- **Suppression d'un utilisateur** : Supprime l'utilisateur et son badge.
- **Changement de mode** : Permet de basculer entre différents modes (par ex. Badge, Empreinte digitale).
- **Historique des accès** : Enregistre l'historique des arrivées/départs et des modifications d'utilisateurs.

## Bibliothèques Principales
- **ESP8266WiFi.h** : Gestion de la connexion WiFi.
- **MFRC522.h** : Communication avec le lecteur RFID.
- **RTClib.h** : Gestion de l'heure via le module RTC.
- **LiquidCrystal_I2C.h** : Affichage sur l'écran LCD (protocole I2C).
- **ESP8266WebServer.h** : Création d'un serveur Web pour la gestion des utilisateurs.
- **LittleFS.h** : Stockage local des fichiers sur l'ESP8266.
- **HTTPSRedirect.h** : Envoi sécurisé des données vers Google Sheets.

## Matériel Utilisé

### Partie Hardware :
- **ESP8266**
- **Lecteur RFID (RC522)**
- **Badges RFID**
- **Module Horloge DS3231**
- **Écran LCD**
- **Module de charge TP4056**
- **Jumpers**

### Partie Software :
- **Arduino IDE**
- **Visual Studio Code (VSCode)**

## Installation

### Étape 1 : Préparer l'ESP8266 avec l'IDE Arduino
1. **Télécharger et installer l'IDE Arduino** depuis [arduino.cc](https://www.arduino.cc/en/software).
2. **Configurer l'ESP8266** :
   - Ouvrir l'IDE Arduino.
   - Aller dans **File > Preferences**.
   - Dans "Additional Board Manager URLs", ajouter :
     ```
     https://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Aller dans **Tools > Board > Board Manager** et installer **ESP8266**.

### Étape 2 : Installer les bibliothèques nécessaires
Dans l'IDE Arduino :
1. Aller dans **Sketch > Include Library > Manage Libraries**.
2. Rechercher et installer les bibliothèques suivantes :
   - **ESP8266WiFi**
   - **MFRC522**
   - **LiquidCrystal_I2C**
   - **Wire**
   - **ESP8266WebServer**
   - **LittleFS**
   - **HTTPSRedirect**
   - **ESP8266HTTPClient**
   - **RTClib**

### Étape 3 : Câblage
#### Connexions du RFID (RC522) avec l'ESP8266 :
| RC522 | ESP8266 |
|-------|---------|
| SDA   | D8      |
| SCK   | D5      |
| MOSI  | D7      |
| MISO  | D6      |
| GND   | GND     |
| 3.3V  | 3.3V    |

#### Connexions du Module Horloge (DS3231) avec l'ESP8266 :
| DS3231 | ESP8266 |
|--------|---------|
| SDA    | D2      |
| SCL    | D1      |
| VCC    | 3.3V    |
| GND    | GND     |

#### Connexions de l'Écran LCD avec l'ESP8266 :
| LCD  | ESP8266 |
|------|---------|
| SCL  | D1      |
| SDA  | D2      |
| VCC  | VIN     |
| GND  | GND     |

### Étape 4 : Préparation du Code

#### 1. Créer un Google Sheets
- Créer une nouvelle feuille Google Sheets avec les onglets suivants :
  - **History Users**
  - **Registered Users**
  - **Deleted Users**

#### 2. Créer un Google Apps Script
- Allez dans **Extensions > Apps Script** sur votre feuille Google Sheets.
- Supprimez le code par défaut et collez le code du fichier **google_script.js**.
- Remplacez `'VOTRE_ID_SHEET'` par l'ID de votre feuille Google (trouvé dans l'URL).

#### 3. Déployer le Script
- Allez dans **Déployer > Nouveau Déploiement**.
- Choisissez le type **Web App**.
- Définissez l'accès pour "Tout le monde".
- Copiez l'URL et l'ID du script pour l'utiliser dans le code Arduino.

### Étape 5 : Utiliser LittleFS pour Stocker Localement
- Suivez le tutoriel [ici](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html) pour configurer LittleFS pour l'ESP8266.

### Étape 6 : Modifier le Code Arduino
- **Modifiez les informations administratives** dans le fichier `ACCES_RFID_EMPREINTE.ino`.
  - Remplacez `Nom_UTILISATEUR` et `MOT_DE_PASSE`.
- **Configurez les identifiants WiFi** :
  - Remplacez `SSID_WIFI` et `PASSWORD` avec vos informations WiFi.
- **Ajoutez l'ID de Google Apps Script** dans le code Arduino :
  - Remplacez `GOOGLE_SCHEET_ID` par l'ID du script Google Apps généré.

### Étape 7 : Téléverser le Code sur l'ESP8266
- Une fois toutes les modifications effectuées, téléversez le code sur l'ESP8266 via l'IDE Arduino.

---

### Notes :
- Ce système fonctionne en mode hors ligne et en ligne, stockant temporairement les données lorsque le réseau est indisponible.
- Vous pouvez personnaliser les modes d'accès (par badge, empreinte digitale, etc.).

