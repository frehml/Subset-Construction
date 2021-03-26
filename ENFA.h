//
// Created by frehml on 26/03/2021.
//

#ifndef SSC_ENFA_H
#define SSC_ENFA_H
#include <string>
#include "DFA.h"
#include "json.hpp"
#include <map>
#include <set>

using namespace std;
using json = nlohmann::json;

class ENFA {
public:
    string path;
    json dfa;
    json enfa;
    string eps;
    set<vector<string>> allStates;

    ENFA(string p);

    DFA toDFA();
    void subsetConstruction(vector<string> state);
    void addState(string name, bool starting, bool accepting);
    void addTransition(string from, string to, string input);
    vector<string> tryEpsilon(vector<string> state1);
    vector<string> findTransition(vector<string> state, string input);
    string vecToString(vector<string> new_state);
    bool accept(vector<string> new_state);
};


#endif //SSC_ENFA_H
