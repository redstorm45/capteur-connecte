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
// Chaînes
const char* urlBrokerMQTT = "url";
const char* urlServeurNTP = "url";
const char* wifiSSID = "wifi";
const char* wifiMdp = "mdp";
const char* mqttUsername = "capteur";
const char* mqttTopic = "topic";

/* *** Objets pour les librairies *** */
// Connexion wi-fi
WiFiClient espClient;
// Connexion MQTT
PubSubClient clientMQTT(espClient, urlBrokerMQTT);
// Lien UDP
WiFiUDP ntpUDP;
// Client NTP pour l'horodatage des données
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000); // offset(s), delai(ms)
// Instance OneWire pour le capteur de température
OneWire oneWire(PIN_CAPTEUR);
// Objet capteurs de températures
DallasTemperature capteurs(&oneWire);
// Objet pour l'addresse du capteur
DeviceAddress adresseCapteur;

/* *** Variables globales *** */
// wi-fi
unsigned long timerWifi;
bool etatWifi = false;
// buffer
Buffer buffer;

/* *** Définitions des fonctions *** */
void callback(const MQTT::Publish& pub);
void setup();
void tickWifi();
String format(Mesure m);
void loop();

/* *** Fonctions *** */
void callback(const MQTT::Publish& pub) {}

void setup() {
  // Initialiser la connexion wifi
  WiFi.begin(wifiSSID, wifiMdp);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  timerWifi = millis();
  etatWifi = true;
  // NTP et MQTT déjà initialisés (objets créés)
  timeClient.begin();
  // Connexion au capteur
  capteurs.begin();
  capteurs.getAddress(adresseCapteur, 0);
  capteurs.setResolution(adresseCapteur, 12);
  // Initialisation du buffer
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

String format(Mesure m){
  return String(m.timestamp)+','+String(m.valeur);
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
      if(buffer.disponible())
        clientMQTT.publish(MQTT::Publish(mqttTopic, format(buffer.popOldestData())).set_qos(1));
      clientMQTT.loop();
    }
  }
  // Mesurer la température
  // Stocker la température
  // Vérifier s'il faut envoyer les données
  // Les envoyer s'il le faut
}



