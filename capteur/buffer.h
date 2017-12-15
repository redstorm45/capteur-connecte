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
    int size;
    int* list;
    int lastlastPosition;
    int firstlastPosition;
  public:
    Buffer(int n);
    void addData(int k);
    int *getLastData(int k);
    int *popLastData(int k);
};

Buffer::Buffer(int n){
  this->size = n; 
  this->lastlastPosition = 0;
  this->list = new int[n];
  
};

#endif
