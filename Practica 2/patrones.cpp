#include <iostream>
#include <list>
#include <vector>
#include <iterator>

using namespace std;

template <typename C>
void Write(C& ds, ostream& os) {
	typename C::iterator it;
	for (it = ds.begin(); it != ds.end(); it++) {
		os << *it << endl;
	}
}

int main() {
	list<float> miLista;
	vector<float> miVector;
	for (int i = 0; i < 10; i++) {
		miLista.push_back(i);
		miVector.push_back(i + 50);
	}
	Write(miLista, cout);
	Write(miVector, cout);
}
