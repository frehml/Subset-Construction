//
// Created by frehml on 26/03/2021.
//

#include "ENFA.h"
#include "json.hpp"
#include <fstream>
#include <map>


using namespace std;
using json = nlohmann::json;

//constructor
ENFA::ENFA(string p) {
    path = p;
    ifstream input(path);
    input >> enfa;
    eps = enfa["eps"];
}

//checkt of state een accepterende state is
bool ENFA::accept(vector<string> new_state) {
    bool accepting = false;
    for (int i = 0; i < enfa["states"].size(); i++) {
        if (enfa["states"][i]["accepting"] == true) {
            if (count(new_state.begin(), new_state.end(), enfa["states"][i]["name"]))
                accepting = true;
        }
    }
    return accepting;
}

//zet vector om in string
string ENFA::vecToString(vector<string> new_state) {
    if (new_state.size() <= 0)
        return "{}";

    string name = "{" + new_state[0];
    for (int i = 1; i < new_state.size(); i++) {
        name += ",";
        name += new_state[i];
    }
    name += "}";
    return name;
}

//voegt state toe aan het json bestand
void ENFA::addState(string name, bool starting, bool accepting){
    dfa["states"].push_back(
            {{"name", name},
             {"starting", starting},
             {"accepting", accepting}});
}

//voegt transitie toe aan het json bestand
void ENFA::addTransition(string from, string to, string input){
    dfa["transitions"].push_back(
            {{"from", from},
             {"to", to},
             {"input", input}});
}

//zoek transitie
vector<string> ENFA::findTransition(vector<string> state, string input){
    vector<string> new_state;
    for(int i = 0; i < enfa["transitions"].size(); i++){
        if(count(state.begin(), state.end(), enfa["transitions"][i]["from"]) && enfa["transitions"][i]["input"] == input){
            new_state.push_back(enfa["transitions"][i]["to"]);
        }
    }
    return new_state;
}

//probeert voor elke state een epsilion transitie
vector<string> ENFA::tryEpsilon(vector<string> state1){
    vector<string> new_state = state1;

    for(auto transition : enfa["transitions"]){
        if(count(state1.begin(), state1.end(), transition["from"]) && transition["input"] == eps)
            new_state.push_back(transition["to"]);
    }
    sort(new_state.begin(), new_state.end());
    new_state.erase( unique( new_state.begin(), new_state.end() ), new_state.end() );

    if(new_state == state1)
        return new_state;
    else
        return tryEpsilon(new_state);
}

//vind ttransities en bijgevolg states recursief
void ENFA::subsetConstruction(vector<string> state) {
    vector<vector<string>> states;

    if(allStates.find(state) != allStates.end())
        return;
    allStates.insert(state);

    for(int i = 0; i < dfa["alphabet"].size(); i++){
        string alph = dfa["alphabet"][i];
        vector<string> new_state = tryEpsilon(findTransition(state, alph));
        states.push_back(new_state);
        addTransition(vecToString(state), vecToString(new_state), alph);
    }

    for(int i = 0; i < states.size(); i++){
        subsetConstruction(states[i]);
    }
}

//zet dfa om in nfa
DFA ENFA::toDFA() {
    vector<string> startState;
    dfa = {
            {"type",     "DFA"},
            {"alphabet", enfa["alphabet"]}
    };

    for (int i = 0; i < enfa["states"].size(); i++) {
        if (enfa["states"][i]["starting"] == true) {
            startState = {enfa["states"][i]["name"]};
            startState = tryEpsilon(startState);
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

    ofstream file(path+".2DFA.json");
    file << dfa;
    return DFA(path+".2DFA.json");
}