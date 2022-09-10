#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>
#include <chrono>

using namespace std;

int eucDist(vector<int> p1, vector<int> p2){
  if (p1.size() != p2.size()) return -1;
  vector<int> temp;
  for (int i = 0; i < p1.size(); i++){
    temp.push_back(pow(p1[i]-p2[i],2));
  }
  int result = 0;
  for (int i = 0; i < temp.size(); i++){
    result += temp[i];
  }
  return sqrt(result);
}

class Point{
public:
  int dims;
  vector<int> coords;
  Point(int d){
    srand(time(NULL));
    dims = d;
    for (int i = 0; i < d; i++){
      int temp = rand()% d*100;
      coords.push_back(temp);
    }
  }
};

int main() {
  vector<Point> puntos;
  int tam, dims;
  cout << "Ingrese cantidad de puntos: ";
  cin >> tam;
  cout << "Ingrese cantidad de dimensiones: ";
  cin >> dims;
  for (int i = 0; i < tam; i++){
    Point z(dims);
    puntos.push_back(z);
  }

  auto start = chrono::high_resolution_clock::now();
  for (int i = 0; i < puntos.size(); i++){
    int dist = eucDist(puntos[0].coords, puntos[i].coords);
  }
  auto end = chrono::high_resolution_clock::now();
  auto duration = duration_cast<chrono::microseconds>(end-start);
  cout << "Tiempo de ejecución: " << duration.count() << " microsegundos."<< endl;
}