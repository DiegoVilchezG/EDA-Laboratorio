#include <iostream>

using namespace std;

class CVector{
private:
  int *cVect, cCount, cMax, cDelta;
  void Init(int cDelta);
  void cResize();
public:
  CVector(int delta = 10){
    cCount = 0;
    cMax = delta;
    cDelta = delta;
  };
  void Insert (int e);

};

void CVector::cResize(){
  int *temp;
  temp = new int[cMax+cDelta];
  for (int i = 0; i < cMax; i++){
    temp[i] = cVect[i];
  }
  delete[] cVect;
  cVect = temp;
}


void CVector::Insert(int e){
  if (cCount == cMax){
    cResize();
  }
  cVect[cCount++] = e;
}

int main(){
  CVector vect(10);
  vect.Insert(5);
}
