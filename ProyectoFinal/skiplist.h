#pragma once
#ifndef SKIP_LIST_H
#define SKIP_LIST_H 

#include <iostream>
#include <time.h>
#include <string>
#include <sstream>
#include <vector>

//============================================================================== 
struct Skip_Node {
    int key;
    std::string value;

    // Punteros a los nodos siguientes
    std::vector<Skip_Node*> forward;

    Skip_Node(int k, const std::string& v, int level);
};

//============================================================================== 
class Skip_list {
public:
    Skip_list();
    ~Skip_list();

    // Funciones no modificadoras
    void print();
    Skip_Node* find(int searchKey);

    // Funciones modificadoras
    void insert(int searchKey, std::string newValue);
    void erase(int searchKey);
private:
    // puntero al primer nodo
    Skip_Node* head;
    // ultimo nodo
    Skip_Node* NIL;

    // Funciones implicitas
    int randomLevel();
    int nodeLevel(const std::vector<Skip_Node*>& v);
    Skip_Node* makeNode(int key, std::string val, int level);

    // variables de valores/datos 
    float probability;
    int maxLevel;
};

Skip_Node::Skip_Node(int k, const std::string& v, int level)
    : key(k), value(v)
{
    for (int i = 0; i < level; ++i) forward.emplace_back(nullptr);
}

//==============================================================================

// constructor
Skip_list::Skip_list()
    : probability(0.5), maxLevel(160)
{
    // Inicializar el Head de la Skiplist

    // Key mas pequeña posible
    constexpr int headKey = std::numeric_limits<int>::min();
    head = new Skip_Node(headKey, "head", maxLevel);

    // Inicializar el ultimo elemento

    // Key mas grande posible
    constexpr int nilKey = std::numeric_limits<int>::max();
    NIL = new Skip_Node(nilKey, "NIL", maxLevel);

    // Conectar inicio con final
    // Conectar todos los niveles/punteros al frente con el NIL
    for (size_t i = 0; i < head->forward.size(); ++i) {
        head->forward[i] = NIL;
    }
}

// destructor
Skip_list::~Skip_list() {
    delete head;
    delete NIL;
}

// Funciones auxiliares
/*
    Funcion: randomLevel()
    Uso implicito en la clase Skip_list
    Genera niveles de nodos en el rango
    [1, maxLevel).

    Usa rand() escalado por su maximo valor:
    RAND_MAX, para que los numeros generados
    esten dentro del intervalo [0,1).
*/

int Skip_list::randomLevel() {
    int v = 1;

    while ((((double)std::rand() / RAND_MAX)) < probability &&
        std::abs(v) < maxLevel) {

        v += 1;
    }
    return abs(v);
}

/*
    Funcion: nodeLevel()
    Uso implicito en la mayoria de las otras funciones.

    Retorna el numero de punteros no nulos
    correspondientes al nivel del nodo actual.
    (el nodo que contiene el vector revisado de punteros
    al frente)

    Si la lista esta vacia simplemente retorna 1.
*/
int Skip_list::nodeLevel(const std::vector<Skip_Node*>& v) {
    int currentLevel = 1;
    // Key del ultimo elemento siempre es la mayor
    constexpr int nilKey = std::numeric_limits<int>::max();

    if (v[0]->key == nilKey) {
        return currentLevel;
    }

    for (size_t i = 0; i < v.size(); ++i) {

        if (v[i] != nullptr && v[i]->key != nilKey) {
            ++currentLevel;
        }
        else {
            break;
        }
    }
    return currentLevel;
}

// Funciones no modificadoras
/*
    Funcion: print()
    Uso: skip_list_obj.print();

    Muestra el key, valor y nivel
    de cada nodo de la skip list.

    Muestra 2 nodos por linea.
*/
void Skip_list::print() {
    Skip_Node* list = head;
    int lineLenght = 1;

    std::cout << "{";

    while (list->forward[0] != nullptr) {
        std::cout << "value: " << list->forward[0]->value
            << ", key: " << list->forward[0]->key
            << ", level: " << nodeLevel(list->forward);

        list = list->forward[0];

        if (list->forward[0] != nullptr) std::cout << " ||| ";

        if (++lineLenght % 2 == 0) std::cout << "\n";
    }
    std::cout << "}\n";
}

/*
    Funcion: find()
    Uso: Skip_Node* found = skip_list_obj.find(searchKey);

    Busca a traves de la skip list y
    retorna el elemento correspondiente
    a searchKey; si no retorna un puntero vacío.
*/

Skip_Node* Skip_list::find(int searchKey) {
    Skip_Node* x = head;
    unsigned int currentMaximum = nodeLevel(head->forward);

    for (unsigned int i = currentMaximum; i-- > 0;) {
        while (x->forward[i] != nullptr && x->forward[i]->key < searchKey) {
            x = x->forward[i];
        }
    }
    x = x->forward[0];

    if (x->key == searchKey) {
        return x;
    }
    else {
        return nullptr;
    }
}

// Funciones modificadoras
/*
    Funcion: makeNode ()
    Uso: Implicito en funcion insert().

    Encapsula el constructor de Skip_Node, creando
    un nodo en el heap y retornando un puntero a este.
*/
Skip_Node* Skip_list::makeNode(int key, std::string val, int level) {
    return new Skip_Node(key, val, level);
}

/*
    Funcion: insert();
    Uso: void insert(searchKey, newValue);

    Busca la skip list, y si encuentra un elemento
    con searchKey como key asignada, el valor se
    reasigna a newValue. Si no, crea y divide un nodo
    nuevo de nivel aleatorio.
*/
void Skip_list::insert(int searchKey, std::string newValue) {
    // reasignar si el nodo existe
    Skip_Node* x = nullptr;
    x = find(searchKey);
    if (x) {
        x->value = newValue;
        return;
    }
    //vector de punteros que necesitan actualizarse para contar el nuevo nodo
    std::vector<Skip_Node*> update(head->forward);
    unsigned int currentMaximum = nodeLevel(head->forward);
    x = head;

    // Buscar la lista 
    for (unsigned int i = currentMaximum; i-- > 0;) {

        while (x->forward[i] != nullptr && x->forward[i]->key < searchKey) {

            x = x->forward[i];
        }
        update[i] = x;
    }
    x = x->forward[0];

    // crear nuevo nodo
    int newNodeLevel = 1;
    if (x->key != searchKey) {

        newNodeLevel = randomLevel();
        int currentLevel = nodeLevel(update);

        if (newNodeLevel > currentLevel) {

            for (int i = currentLevel + 1; i < newNodeLevel; ++i) {

                update[i] = head;
            }
        }
        x = makeNode(searchKey, newValue, newNodeLevel);
    }

    // conectar punteros de los predecesores y nodo actual con los sucesores
    for (int i = 0; i < newNodeLevel; ++i) {

        x->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = x;
    }
}

/*
    Funcion: delete_node()
    Uso: skip_list_obj.erase(searchKey)

    Elimina el elemento que contenga a searchKey,
    si es que este existe.
*/
void Skip_list::erase(int searchKey) {
    // Vector de punteros que necesitan actualizarse para contar el nodo eliminado
    std::vector<Skip_Node*> update(head->forward);
    Skip_Node* x = head;
    unsigned int currentMaximum = nodeLevel(head->forward);

    // buscar y actualizar punteros
    for (unsigned int i = currentMaximum; i-- > 0;) {

        while (x->forward[i] != nullptr && x->forward[i]->key < searchKey) {

            x = x->forward[i];
        }
        update[i] = x;
    }
    x = x->forward[0];

    // actualizar punteros y borrar nodo
    if (x->key == searchKey) {
        for (size_t i = 0; i < update.size(); ++i) {

            if (update[i]->forward[i] != x) {

                break;
            }
            update[i]->forward[i] = x->forward[i];
        }
        delete x;
    }
}

#endif