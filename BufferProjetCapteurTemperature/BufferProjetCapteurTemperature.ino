class Buffer{
  private:
    int size;
    int* list;
    int position;
  public:
    Buffer(int n);
    void addData(int k);
    int *getLastData(int k);
};

Buffer::Buffer(int n){
  this->size = n; 
  this->position = 0;
  this->list = new int[n];
  
};


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
  this->list[this->position] = this->size;
  this->position += 1;
  this->position = this->position%this->size;
  return;
};



