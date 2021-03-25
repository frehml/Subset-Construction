#ifndef OPDRACH1_DFA_H
#define OPDRACH1_DFA_H

#include <string>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class DFA {

public:
    string path;
    json dfa;
    DFA(string p);

    bool accepts(string numbers);
    void print();
};


#endif //OPDRACH1_DFA_H
