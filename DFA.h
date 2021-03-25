#ifndef OPDRACH1_DFA_H
#define OPDRACH1_DFA_H

#include <string>
#include <iostream>
using namespace std;


class DFA {

public:
    string path;
    DFA(string p){ path = p;};

    bool accepts(string numbers);
    void print();
};


#endif //OPDRACH1_DFA_H
