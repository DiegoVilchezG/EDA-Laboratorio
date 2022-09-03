#include <iostream>

using namespace std;

template <typename T>
class CVector {
	T* pVect;
	int pCount, pMax, pDelta;
	void Resize();

public:
	CVector(int d = 10) {
		pVect = new T[d];
		pCount = 0;
		pMax = d;
		pDelta = d;
	}
	void Insert(T e);
};

template <typename T>
void CVector<T>::Insert(T e) {
	if (pCount == pMax) {
		Resize();
	}
	pVect[pCount++] = e;
}

template <typename T>
void CVector<T>::Resize() {
	int* temp;
	temp = new int[pMax + pDelta];
	for (int i = 0; i < pMax; i++) {
		temp[i] = pVect[i];
	}
	delete[] pVect;
	pVect = temp;
}

int main() {
	CVector<int> vector;
	vector.Insert(7);
}
