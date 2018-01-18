#ifndef BUFFER_H
#define BUFFER_H

// Buffer
#define LONGUEUR_BUFFER  512

/* Structure d'une mesure */
struct Mesure
{
  float valeur;
  unsigned long timestamp;
};


class Buffer{
  private:
    Mesure* list[LONGUEUR_BUFFER];
    int lastPosition;  // position de l'élément le plus ancien
    int firstPosition; // première position libre
    bool inLightMode;
  public:
    Buffer();
    bool addData(Mesure* k);
    bool isInLightMode();
    bool disponible();
    Mesure* popOldestData();
};

#endif
