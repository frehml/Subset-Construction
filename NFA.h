//
// Created by Frederic frehml on 25/02/2021.
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

    explicit NFA(string p);

    DFA toDFA();
    void subsetConstruction(vector<string> const &state);
    void addState(string name, bool starting, bool accepting);
    void addTransition(string from, string to, string input);
    vector<string> findTransition(vector<string> state, string input);
    static string vecToString(vector<string> new_state);
    bool accept(vector<string> new_state);
};


#endif //SSC_NFA_H
