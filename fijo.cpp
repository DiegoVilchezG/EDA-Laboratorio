#include <iostream>

using namespace std;

int vect[100];
int nCount;

void fInsert(int e){
  if (nCount < 100){
    vect[nCount++] = e;
  }
}

int main(){
  fInsert(2);
}
