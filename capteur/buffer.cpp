#include "buffer.h"
#include "Arduino.h"

Buffer::Buffer(){
  
  for(int i=0;i<LONGUEUR_BUFFER;i++)
    list[i] = NULL;
  lastPosition = 0;
  firstPosition = 0;
  inLightMode = false;
}

bool Buffer::disponible(){
  return this->list[this->firstPosition] == NULL;
}
bool Buffer::isInLightMode(){
  return this->inLightMode;
}

bool Buffer::addData(Mesure* k){
  this->list[this->firstPosition] = k;
  
  this->firstPosition = (this->firstPosition + 1)%LONGUEUR_BUFFER;
  
  // écrase les données trop anciennes si nécéssaire
  
  if(this->firstPosition == this->lastPosition){
    this->lastPosition = (this->lastPosition + 1)%LONGUEUR_BUFFER;
    this->inLightMode = true;
    return true;
  }
  return false;
};

Mesure* Buffer::popOldestData(){
  // le code d'appel doit vérifier qu'une donnée est présente avant!
  // (à l'aide de la méthode disponible() )
  this->inLightMode = false;
  Mesure* m=this->list[this->lastPosition];
  this->list[this->lastPosition] = NULL;
  this->lastPosition = (this->lastPosition + 1)%LONGUEUR_BUFFER;
  return m;
}
