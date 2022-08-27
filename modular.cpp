#include <iostream>

using namespace std;

struct Vector{
  int *mpVect, mnCount, mnMax, mnDelta;
};

void mResize(Vector *pThis){
    const int delta = 10;
    int *pTemp;
    pTemp = new int[pThis->mnMax+pThis->mnDelta];
    for (int i = 0; i < pThis->mnMax; i++){
      pTemp[i] = pThis->mpVect[i];
    }
    delete [] pThis->mpVect;
    pThis->mpVect = pTemp;

    pThis->mnMax += pThis->mnDelta;
}

void mInsert(Vector *pThis, int e){
  if (pThis->mnCount == pThis->mnMax){
    mResize(pThis);
  }
  pThis->mpVect[pThis->mnCount++] = e;
}

int main(){
  Vector *vect;
  mInsert(vect, 78);
}
