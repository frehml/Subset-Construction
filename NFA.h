//
// Created by Frederic Hamelink on 25/02/2021.
//

#ifndef SSC_NFA_H
#define SSC_NFA_H
#include <string>
#include "DFA.h"
#include "json.hpp"
#include <map>

using namespace std;
using json = nlohmann::json;

class NFA {
public:
    string path;
    json dfa;
    json nfa;
    map<string, string> trans;

    NFA(string p);

    DFA toDFA();
    void findStates(vector<string> state);
    string vecToString(vector<string> new_state);
    bool accept(vector<string> new_state);
};


#endif //SSC_NFA_H
