#include "buffer.h"

Buffer::Buffer(){
  for(int i=0;i<LONGUEUR_BUFFER;i++)
    list[i] = null;
  lastPosition = 0;
  firstPosition = 0;
}

bool Buffer::disponible(){
  return this->lastPosition != this->firstPosition;
}

int *Buffer::getLastData(int k){
  if(k>this->size or k<=0){
    //Lancer une exception
    
  }else{
    int* dataArray= new int[k];
    for(int i=0;i++;i<k){
      dataArray[i] = list[this->size-k+i];
    };
    return *dataArray;
  }
  
};

bool Buffer::addData(Mesure k){
  this->list[this->firstPosition] = k;
  this->firstPosition = (this->firstPosition + 1)%LONGUEUR_BUFFER;
  // écrase les données trop anciennes si nécéssaire
  if(this->firstPosition == this->lastPosition){
    this->lastPosition = (this->lastPosition + 1)%LONGUEUR_BUFFER;
    return true;
  }
  return false;
};

Mesure Buffer::popOldestData(){
  // le code d'appel doit vérifier qu'une donnée est présente avant!
  // (à l'aide de la méthode disponible() )
  Mesure m=this->list[this->lastPosition];
  this->lastPosition = (this->lastPosition + 1)%LONGUEUR_BUFFER;
}
