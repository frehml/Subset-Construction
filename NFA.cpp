//
// Created by Frederic Hamelink on 25/02/2021.
//

#include "NFA.h"
#include "json.hpp"
#include <fstream>
#include <map>
#include <any>

using namespace std;
using json = nlohmann::json;

//constructor
NFA::NFA(string p) {
    path = p;
    ifstream input(path);
    input >> nfa;
}

//checkt of state een accepterende state is
bool NFA::accept(vector<string> new_state) {
    bool accepting = false;
    for (int i = 0; i < nfa["states"].size(); i++) {
        if (nfa["states"][i]["accepting"] == true) {
            if (count(new_state.begin(), new_state.end(), nfa["states"][i]["name"]))
                accepting = true;
        }
    }
    return accepting;
}

//zet vector om in string
string NFA::vecToString(vector<string> new_state) {
    string name = "{" + new_state[0];
    for (int i = 1; i < new_state.size(); i++) {
        name += ",";
        name += new_state[i];
    }
    name += "}";
    return name;
}

//vind states en transities recursief
void NFA::findStates(vector<string> state) {
    vector<string> new_state;
    //loopt over alphabet
    for (int x = 0; x < nfa["alphabet"].size(); x++) {
        //loopt over transities
        for (int y = 0; y < nfa["transitions"].size(); y++) {
            //als gegeven transitie en gegeven alphabet overeenkomen dan voegen we die toe aan new state
            if (count(state.begin(), state.end(), nfa["transitions"][y]["from"]) &&
                nfa["transitions"][y]["input"] == nfa["alphabet"][x])
                new_state.push_back(nfa["transitions"][y]["to"]);
        }

        //sort de state alfabetsich of numeriek
        sort(new_state.begin(), new_state.end());
        //zet vector om tot string
        string name = vecToString(new_state);
        //checkt of een state in de vector accepting is
        bool accepting = accept(new_state);
        //voegt nieuwe transitie toe aan transities
        dfa["transitions"].push_back(
                {{"from",  vecToString(state)},
                 {"to",    name},
                 {"input", nfa["alphabet"][x]}});
        //check (base case)
        if (trans[name] == nfa["alphabet"][x])
            continue;
        //voeg niewe state toe aan states
        dfa["states"].push_back(
                {{"name",      name},
                 {"starting",  false},
                 {"accepting", accepting}});
        //voeg transitie toe aan trans voor check
        trans[name] = nfa["alphabet"][x];
        findStates(new_state);
    }
    return;
}

DFA NFA::toDFA() {
    dfa = {
            {"type",     "DFA"},
            {"alphabet", nfa["alphabet"]}
    };

    vector<string> startState;

    for (int i = 0; i < nfa["states"].size(); i++) {
        if (nfa["states"][i]["starting"] == true) {
            startState = {nfa["states"][i]["name"]};
            dfa["states"] = {"", {{"name", vecToString({nfa["states"][i]["name"]})},
                                  {"starting", true},
                                  {"accepting", nfa["states"][i]["accepting"]}}};
        }
    }

    dfa["transitions"] = {"", ""};
    findStates(startState);

    //verwijder de blank spaces
    dfa["states"].erase(dfa["states"].begin());
    dfa["transitions"].erase(dfa["transitions"].begin());
    dfa["transitions"].erase(dfa["transitions"].begin());
    ofstream file("DFA.json");
    file << dfa;
    return DFA("DFA.json");
}