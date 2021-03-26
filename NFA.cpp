
//
// Created by Frederic Hamelink on 25/02/2021.
//

#include "NFA.h"
#include "json.hpp"
#include <fstream>
#include <map>


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

void NFA::addState(string name, bool starting, bool accepting){
    dfa["states"].push_back(
                        {{"name", name},
                             {"starting", starting},
                             {"accepting", accepting}});
}

void NFA::addTransition(string from, string to, string input){
    dfa["transitions"].push_back(
                        {{"from", from},
                             {"to", to},
                             {"input", input}});
}

vector<string> NFA::findTransition(vector<string> state, string input){
    vector<string> new_state;
    for(int i = 0; i < nfa["transitions"].size(); i++){
        if(count(state.begin(), state.end(), nfa["transitions"][i]["from"]) && nfa["transitions"][i]["input"] == input){
            new_state.push_back(nfa["transitions"][i]["to"]);
        }
    }
    sort(new_state.begin(), new_state.end());
    new_state.erase( unique( new_state.begin(), new_state.end() ), new_state.end() );
    addTransition(vecToString(state), vecToString(new_state), input);
    return new_state;
}

//vind states en transities recursief
void NFA::subsetConstruction(vector<string> state) {
    vector<vector<string>> states;

    if(allStates.find(state) != allStates.end())
        return;
    allStates.insert(state);

    for(int i = 0; i < dfa["alphabet"].size(); i++){
        states.push_back(findTransition(state, dfa["alphabet"][i]));
    }

    for(int i = 0; i < states.size(); i++){
        subsetConstruction(states[i]);
    }
}

DFA NFA::toDFA() {
    vector<string> startState;
    dfa = {
            {"type",     "DFA"},
            {"alphabet", nfa["alphabet"]}
    };

    for (int i = 0; i < nfa["states"].size(); i++) {
        if (nfa["states"][i]["starting"] == true) {
            startState = {nfa["states"][i]["name"]};
            dfa["states"] = {"", ""};
        }
    }

    dfa["transitions"] = {"", ""};
    subsetConstruction(startState);

    for(auto elem : allStates){
        if(elem == startState)
            addState(vecToString(elem), true, accept(elem));
        else
            addState(vecToString(elem), false, accept(elem));
    }

    //verwijder de blank spaces
    dfa["states"].erase(dfa["states"].begin());
    dfa["states"].erase(dfa["states"].begin());
    dfa["transitions"].erase(dfa["transitions"].begin());
    dfa["transitions"].erase(dfa["transitions"].begin());

    ofstream file("DFA.json");
    file << dfa;
    return DFA("DFA.json");
}