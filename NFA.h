//
// Created by Frederic Hamelink on 25/02/2021.
//

#ifndef SSC_NFA_H
#define SSC_NFA_H
#include <string>
#include "DFA.h"
#include "json.hpp"
#include <map>
#include <set>

using namespace std;
using json = nlohmann::json;

class NFA {
public:
    string path;
    json dfa;
    json nfa;
    set<vector<string>> allStates;

    NFA(string p);

    DFA toDFA();
    void subsetConstruction(vector<string> state);
    void addState(string name, bool starting, bool accepting);
    void addTransition(string from, string to, string input);
    vector<string> findTransition(vector<string> state, string input);
    string vecToString(vector<string> new_state);
    bool accept(vector<string> new_state);
};


#endif //SSC_NFA_H
