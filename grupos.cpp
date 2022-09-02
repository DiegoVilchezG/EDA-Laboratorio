#include <iostream>
#include <list>
#include <vector>

using namespace std;

class CDataStruct {

	vector<float> cont;
public:
	void Insert(float fVal) {
		cont.push_back(fVal);
	}
	
	template <typename c>
	void sumone(c funobj) {
		vector<float>::iterator iter = cont.begin();
		for (; iter != cont.end(); iter++)
			funobj(*iter);
	}
};

template <typename o>
class func {
public:
	void operator()(o& inst) {
		inst++;
	}
};

int main() {
	CDataStruct ds;
	ds.Insert(3.5);
	ds.Insert(4.5);
	ds.Insert(6.5);
	ds.Insert(3.9);

	func<float> x;
	ds.sumone(x);
}
