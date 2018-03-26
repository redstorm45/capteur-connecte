# Introduction

Code arduino et python du projet de capteur connecté.<br/>
Il est fait pour utilisation avec un ESP8266 (ESP-01), mais est sûrement utilisable pour d'autres modèles d'ESP.<br/>
Il est aussi fait pour utiliser un capteur Dallas DS18B20, communiquant avec le protocole One-Wire.

## Librairies

Le programme utilise les librairies suivantes:
- [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library)
- [NTPClient](https://github.com/arduino-libraries/NTPClient)
- OneWire
- [PubSubClient](https://github.com/knolleary/pubsubclient)

# Vue générale

## Architecture des transferts de données

Les données provennant du capteur sont captées par le programme à travers la bibliothèque OneWire, et converties en nombre à virgule flottante par la bibliothèque DallasTemperature.<br/>
À la réception, les données sont datés à l'aide de la bibliothèque NTPClient, et stockées dans un tampon temporaire avant envoi.<br/>
À interval fixe, le programme tente de vider tout le tampon de mesures, par des envois successifs à l'aide de MQTT, par la bibliothèque PubSubClient. Néanmoins, afin de ne pas trop impacter les prises de mesures, il ne s'effectue qu'un envoi par boucle principale, alors que cette même boucle vérifie à chaque fois s'il faut prendre une mesure, et fait fonctionner la pile TCP de l'ESP8266 au besoin.

## Modes de fonctionnement

## Configuration

En haut du programme sont définies les différentes constantes et variables utilisées pour le projet,
ainsi que les options de configuration.

### Constantes

Ces constantes permettent de définir les différents délais ainsi que les interfaces du programme, afin de pouvoir le régler selon l'application voulue.

| Nom                        | Unité | Fonction                                                                                      |
|----------------------------|-------|-----------------------------------------------------------------------------------------------|
| PIN_CAPTEUR                | N/A   | Identifiant du pin sur lequel est connecté le capteur Dallas DS18B20                          |
| DELAI\_MESURE              | ms    | Délai entre chaque mesure en fonctionnement normal                                            |
| DELAI\_MESURE\_ECRASANT    | ms    | Délai entre chaque mesure en fonctionnement dégradé                                           |
| DELAI\_MESURE\_CONVERSION  | ms    | Délai permettant au capteur de réaliser sa mesure, avant de la lui demander                   |
| DELAI_ENVOI                | ms    | Délai entre chaque tentative d'envoi des données                                              |
| DELAI_NTP                  | ms    | Délai entre chaque synchronisation avec le serveur NTP                                        |
| OFFSET_NTP                 | s     | Décalage horaire entre l'heure NTP (UTC) et le fuseau actuel                                  |
| LONGUEUR_BUFFER (buffer.h) | N/A   | Défini la taille du buffer à utiliser, à choisir suivant la place restante pour les variables |

### Fonctionnement

Ces constantes permettent d'appliquer une compilation différente au programme, donc un fonctionnement différent.

| Nom         | Fonction (si définie)                                                                                                           |
|-------------|---------------------------------------------------------------------------------------------------------------------------------|
| DEBUG       | Ajoute des envois de messages par le port série sur divers événements                                                           |
| NTP_SERVER  | Utilise un serveur NTP, plutôt que d'attendre l'heure sous la forme d'un timestamp transmis par MQTT sur le channel "timestamp" |

# Programmation sur un ESP8266 (ESP-01)

## Position des pins

Voici l'arrangement des pins sur un ESP-01, vu de dessus

|:-------:|-------|-------|
|  Reste  | RX    | VCC   |
|   de    | GPIO0 | RST   |
|   la    | GPIO2 | CH_PD |
|  carte  | GND   | TX    |
|---------|-------|-------|

## Connexion

On connecte les pins suivants sur un convertisseur USB <> Port série quelconque. Un Arduino peut remplir cette fonction, sans avoir à utiliser le sketch "Arduino as ISP", en fixant son pin de reset à la masse. Il ne semble pas nécessaire d'utiliser un convertisseur de niveau logique pour éviter d'abîmer la carte.

| Pin ESP8266   | Connecté à                                             |
|---------------|--------------------------------------------------------|
| RX            | TX du convertisseur série                              |
| GPIO0         | pulldown pour programmer, pullup pour lancement normal |
| GPIO2         | pullup + capteur DS18B20                               |
| GND           | Masse                                                  |
| VCC           | Alimentation +3.3V                                     |
| RST           | pullup                                                 |
| CH_PD         | pullup                                                 |
| TX            | RX du convertisseur série                              |

## Envoi du programme

Avant d'envoyer, on effectue une mise à la masse du pin de reset (RST), pour s'assurer que l'ESP n'est pas en train d'executer un programme.
On s'assure aussi d'avoir choisi la carte "Generic ESP8266 Module" dans l'IDE Arduino, pour lequel il faut avoir installé les bibliothèques de programmation pour l'ESP dans le gestionnaire de cartes. On vérifie enfin que le port série choisi est valide. Finalement, on peut envoyer le programme à l'aide du bouton "téléverser".
