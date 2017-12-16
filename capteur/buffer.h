#ifndef BUFFER_H
#define BUFFER_H

// Buffer
#define LONGUEUR_BUFFER  64

/* Structure d'une mesure */
struct Mesure
{
  int valeur;
  unsigned long timestamp;
};


class Buffer{
  private:
    Mesure list[LONGUEUR_BUFFER];
    int lastPosition;  // position de l'élément le plus ancien
    int firstPosition; // première position libre
  public:
    Buffer();
    void addData(Mesure k);
    bool disponible();
    Mesure popOldestData();
};

#endif