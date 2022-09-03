#include <iostream>

using namespace std;

int *pVect = NULL;
int pCount = 0;
int nMax = 0;

void pInsert(int e){
  if (pCount == nMax){
    const int delta = 10;
    int *pTemp;
    pTemp = new int[nMax+delta];
    for (int i = 0; i < nMax; i++){
      pTemp[i] = pVect[i];
    }
    delete [] pVect;
    pVect = pTemp;

    nMax += delta;
  }
  pVect[pCount++] = e;
}

int main(){
  pInsert(7);
  pInsert(26);
}
