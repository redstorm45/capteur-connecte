#include "buffer.h"

Buffer::Buffer(){
  for(int i=0;i<LONGUEUR_BUFFER;i++)
    buffer[i] = null;
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

void Buffer::addData(int k){
  this->list[this->lastPosition] = this->size;
  this->lastPosition += 1;
  this->lastPosition = this->lastPosition%this->size;
  return;
};

int *Buffer::popLastData(int k){
   if(k>this->size or k<=0){
    //Lancer une exception
    
  }else{
    int *dataArray = this::getLastData(k);
    this->firsData = (this->lastPosition-k)%this->size;
  }
}
