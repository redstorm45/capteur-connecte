#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <PubSubClient.h>      // https://github.com/Imroy/pubsubclient
#include <NTPClient.h>         // https://github.com/arduino-libraries/NTPClient
#include <DallasTemperature.h> // https://github.com/milesburton/Arduino-Temperature-Control-Library
#include "buffer.h"

/* *** Paramètres du programme *** */
// Pins
#define PIN_CAPTEUR  2
// Délai entre chaque mesure (ms)
#define DELAI_MESURE  60000
#define DELAI_MESURE_ECRASANT 900000
#define DELAI_ENVOI 900000
#define DELAI_NTP 60000 // délai (ms)
#define OFFSET_NTP 3600 // offset(s)
// Chaînes
const char* urlBrokerMQTT = "url";
const char* urlServeurNTP = "url";
const char* wifiSSID = "ssid";
const char* wifiMdp = "mdp";
const char* mqttUsername = "user";
const char* mqttTopic = "topic";

/* *** Objets pour les librairies *** */
// Connexion wi-fi
WiFiClient espClient;
// Connexion MQTT
PubSubClient clientMQTT(espClient, urlBrokerMQTT);
// Lien UDP
WiFiUDP ntpUDP;
// Client NTP pour l'horodatage des données
NTPClient timeClient(ntpUDP, urlServeurNTP, OFFSET_NTP, DELAI_NTP); 

// Instance OneWire pour le capteur de température
OneWire oneWire(PIN_CAPTEUR);

// Objet capteurs de températures
DallasTemperature capteurs(&oneWire);

// Objet pour l'addresse du capteur
DeviceAddress adresseCapteur;

/* *** Variables globales *** */
// wi-fi
bool etatWifi = false;
// buffer
Buffer buffer;
// mesures

unsigned long timerMesure;
unsigned long timerEnvoi;

/* *** Définitions des fonctions *** */
void callback(const MQTT::Publish& pub);
void setup();
void tickWifi();
String format(Mesure m);
Mesure* capte();
void loop();

/* *** Fonctions *** */
void callback(const MQTT::Publish& pub) {}

void setup() {
  // Initialiser la connexion wifi
  WiFi.begin(wifiSSID, wifiMdp);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  etatWifi = true;
  // NTP et MQTT déjà initialisés (objets créés)
  timeClient.begin();
  // Connexion au capteur
  capteurs.begin();
  capteurs.getAddress(adresseCapteur, 0);
  capteurs.setResolution(adresseCapteur, 12);
  timerMesure = millis() - DELAI_MESURE; // On force la première mesure
  timerEnvoi = millis();
}

void tickWifi(){
  if(etatWifi and WiFi.status() != WL_CONNECTED){
    // on vient de perde la connexion
    WiFi.disconnect();
    WiFi.begin(wifiSSID, wifiMdp);
    etatWifi = false;
  }else if(!etatWifi and WiFi.status() == WL_CONNECTED){
    etatWifi = true;
  }
}

String format(Mesure* m){
  return String(m->timestamp)+','+String(m->valeur);
}

Mesure* capte(){
  capteurs.requestTemperatures();
  Mesure* m=new Mesure;
  (*m) = {.valeur = capteurs.getTempC(adresseCapteur), .timestamp = timeClient.getEpochTime()};
  return m;
}

void loop() {
  // Wi-fi
  tickWifi();
  // NTP
  timeClient.update();
  // MQTT
  if (WiFi.status() == WL_CONNECTED) {
    if (!clientMQTT.connected()) {
      if(clientMQTT.connect(mqttUsername)){
        clientMQTT.set_callback(callback);
      }
    }
    else{
      if(millis()-timerEnvoi >= DELAI_ENVOI){
        if(buffer.disponible()){
          
          Mesure* m = buffer.popOldestData();
          clientMQTT.publish(MQTT::Publish(mqttTopic, format(m)).set_qos(1));
          delete m;

          
        }else{
          
          //Une fois que toutes les données ont été envoyée, on considere que c'est la fin du paquet.
          timerEnvoi = millis();          
          
        }
      }
      clientMQTT.loop();
    }
  }
  // Capteur
  if(millis()-timerMesure >= buffer.isInLightMode()?DELAI_MESURE_ECRASANT:DELAI_MESURE){
    timerMesure += buffer.isInLightMode()?DELAI_MESURE_ECRASANT:DELAI_MESURE;
    Mesure* m=capte();
    buffer.addData(m);
  }
}



