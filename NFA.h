//
// Created by Frederic Hamelink on 25/02/2021.
//

#ifndef SSC_NFA_H
#define SSC_NFA_H
#include <string>
#include "DFA.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class NFA {
public:
    string path;
    json dfa;
    json nfa;

    NFA(string p);

    DFA toDFA();
    void findStates(vector<string> state);
};


#endif //SSC_NFA_H
