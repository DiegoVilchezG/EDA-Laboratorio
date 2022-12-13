#include "skiplist.h"
#include "concurrent.h"
#include <chrono>
#include <time.h>

//Variables Globales
int maxElems = 10;
size_t numThreads = 8;
vector<int> inserts, deletes, gets;
vector<pair<int, int>> ranges;
Skip_list s;
CSkipList cs(maxElems, 0.5);

//Funciones de operacion para la skiplist concurrente

void SLAdd(size_t s, size_t e) {
    if (e >= inserts.size()) {
        e = inserts.size();
    }
    if (s == e) {
        cs.add(inserts[s], to_string(inserts[s]));
    }
    for (size_t i = s; i < e; i++) {
        cs.add(inserts[i], to_string(inserts[i]));
    }
}

int main() {
    srand(time(NULL));

    for (int i = 0; i < maxElems; i++) {
        inserts.push_back(i);
        if (rand() % 4 == 0) {
            deletes.push_back(i);
            gets.push_back(i);
        }
    }

    for (int i = 0; i < numThreads; i++) {
        int a = (rand() % static_cast<int> (maxElems + 1));
        int b = a + (rand() % static_cast<int> (maxElems + 1));
        ranges.push_back(make_pair(a, b));
    }

    //Inserciones secuenciales
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < maxElems; ++i) {

        std::stringstream ss;
        ss << inserts[i];

        s.insert(inserts[i], ss.str());
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Secuencial: " << duration.count() << "ms" << endl;

    //Inserciones con threads
    vector<thread> threads;
    int tam = ceil(float(inserts.size()) / numThreads);
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < inserts.size(); i += tam) {
        threads.push_back(thread(SLAdd, i, i + tam));
    }
    for (auto& th : threads) {
        th.join();
    }
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Threads: " << duration.count() << "ms" << endl;
}
