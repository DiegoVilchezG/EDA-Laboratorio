#include <iostream>

using namespace std;

typedef float (*fOper)(float, float);

float Add(float a, float b){
  return a+b;
}
float Subtract(float a, float b){
  return a-b;
}
float Multiply(float a, float b){
  return a*b;
}
float Divide(float a, float b){
  if (b == 0){
    return -1;
  }
  return a/b;
}

int main(){
  fOper ops[4] = { &::Add, &::Subtract, &::Multiply, &::Divide };
  float a,b,c;
  int opt;
  cin >> a >> b;
  cout << "Operaciones: 0->Suma, 1->Resta, 2->Multiplicación, 3->División: ";
  cin >> opt;
  c = ops[opt](a,b);
  cout << c << endl;
}
