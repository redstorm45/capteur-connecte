#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <PubSubClient.h>      // https://github.com/knolleary/pubsubclient
#include <NTPClient.h>         // https://github.com/arduino-libraries/NTPClient
#include <DallasTemperature.h> // https://github.com/milesburton/Arduino-Temperature-Control-Library
#include "buffer.h"

/* *** Paramètres du programme *** */
// Pins
#define PIN_CAPTEUR  2
// Délai entre chaque mesure (ms)
#define DELAI_MESURE  30000
#define DELAI_MESURE_ECRASANT 300000
#define DELAI_MESURE_CONVERSION 800
#define DELAI_ENVOI 150000
#define DELAI_NTP 600000 // délai (ms)
#define OFFSET_NTP 0     // offset(s) 0: on veut le temps universel
// Mode débug
//#define DEBUG
#define NTP_SERVER

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
PubSubClient clientMQTT(espClient);
// Lien UDP
#ifdef NTP_SERVER
 WiFiUDP ntpUDP;
 // Client NTP pour l'horodatage des données
 NTPClient timeClient(ntpUDP, urlServeurNTP, OFFSET_NTP, DELAI_NTP);
#else
 unsigned long lastTSUpdate;
 unsigned long currentTS;
#endif

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
bool lastMesureDone;

/* *** Définitions des fonctions *** */
void callback(char* topic, byte* payload, unsigned int length);
void setup();
void tickWifi();
String format(Mesure m);
Mesure* capte();
void loop();

/* *** Fonctions *** */
void callback(char* topic, byte* payload, unsigned int length) {
  #ifndef NTP_SERVER 
  if(strcmp(topic, "timestamp")==0){
    unsigned long newTS = 0;
    for(int i=0; i<length; i++){
      newTS = newTS*10 + payload[i] - int('0');
    }
    Serial.println("got ts:"+String(newTS));
    lastTSUpdate = millis();
    currentTS = newTS;
  }
  #endif
}

void setup() {
  // Initialiser la connexion wifi
  WiFi.begin(wifiSSID, wifiMdp);
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("MAC Address:"+WiFi.macAddress());
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    #ifdef DEBUG
      Serial.print(WiFi.status());
    #endif
    delay(500);
  }
  #ifdef DEBUG
    Serial.println("connecté");
    Serial.println(WiFi.localIP());
  #endif
  etatWifi = true;
  // NTP et MQTT déjà initialisés (objets créés)
  #ifdef NTP_SERVER
    timeClient.begin();
  #endif
  clientMQTT.setServer(urlBrokerMQTT, 1883);
  clientMQTT.setCallback(callback);
  delay(500);
  // Connexion au capteur
  capteurs.begin();
  capteurs.getAddress(adresseCapteur, 0);
  capteurs.setResolution(adresseCapteur, 12);
  timerMesure = millis() - DELAI_MESURE; // On force la première mesure
  timerEnvoi = millis();
  #ifdef DEBUG
    Serial.println("initialisé");
  #endif
}

void tickWifi(){
  if(etatWifi and WiFi.status() != WL_CONNECTED){
    // on vient de perde la connexion
    etatWifi = false;
  }else if(!etatWifi and WiFi.status() == WL_CONNECTED){
    etatWifi = true;
  }
}

String format(Mesure* m){
  return String(m->timestamp)+','+String(m->valeur);
}

Mesure* capte(){
  Mesure* m=new Mesure;
  #ifdef NTP_SERVER
    unsigned long TS = timeClient.getEpochTime();
  #else
    unsigned long TS = currentTS+(unsigned long)(millis()-lastTSUpdate)/1000;
  #endif
  (*m) = {.valeur = capteurs.getTempC(adresseCapteur), .timestamp = TS};
  return m;
}

void loop() {
  // Wi-fi
  tickWifi();
  // NTP
  #ifdef NTP_SERVER
    timeClient.update();
  #endif
  // MQTT
  if (WiFi.status() == WL_CONNECTED) {
    if (!clientMQTT.connected()) {
      #ifdef DEBUG
        Serial.println("reconnection au broker mqtt...");
      #endif
      if(clientMQTT.connect(mqttUsername)){
        clientMQTT.subscribe("timestamp");
        #ifdef DEBUG
          Serial.println("connecté au broker mqtt");
        #endif
      }
      #ifdef DEBUG
      else{
         Serial.println("connection au broker mqtt impossible");
      }
      #endif
    }
    else{
      clientMQTT.loop();
      if((millis()-timerEnvoi) >= DELAI_ENVOI){
        #ifdef DEBUG
          Serial.println("envoi possible");
        #endif
        if(buffer.disponible()){
          #ifdef DEBUG
            Serial.println("envoi!");
          #endif
          Mesure* m = buffer.popOldestData();
          String s=format(m);
          byte* data=(byte*)malloc(s.length()+1);
          s.getBytes(data, s.length()+1);
          clientMQTT.publish(mqttTopic, data, s.length()+1);
          free(data);
          delete m;
        }else{
          //Une fois que toutes les données ont été envoyée, on considere que c'est la fin du paquet.
          timerEnvoi = millis();
        }
      }
    }
  }
  // Capteur
  if((millis()-timerMesure) >= (buffer.isInLightMode()?DELAI_MESURE_ECRASANT:DELAI_MESURE)){
    timerMesure += buffer.isInLightMode()?DELAI_MESURE_ECRASANT:DELAI_MESURE;
    capteurs.requestTemperatures();
    lastMesureDone = true;
    #ifdef DEBUG
      Serial.println("request mesure");
    #endif
  }
  if(((millis()-timerMesure) >= DELAI_MESURE_CONVERSION) && lastMesureDone){
    lastMesureDone = false;
    Mesure* m=capte();
    buffer.addData(m);
    #ifdef DEBUG
      Serial.println("mesure!");
    #endif
  }
  // Fonctionnement de la pile TCP de l'ESP8266
  yield();
}



