//
// Created by Frederic Hamelink on 25/02/2021.
//

#include "NFA.h"
#include "json.hpp"
#include <fstream>
#include <map>

using namespace std;
using json = nlohmann::json;

NFA::NFA(string p){
    path = p;
    ifstream input(path);
    input >> nfa;
}

void NFA::findStates(vector<string> state){
    vector<string> new_state;

    //print for testing
    for(int i = 0; i < state.size(); i ++)
        cout << state[i] << endl;
    cout << endl;

    //base case
    if(state[1] == "4")
        return;

    //loopt over alphabet
    for (int x = 0; x < nfa["alphabet"].size(); x++) {
        //loopt over transities
        for (int y = 0; y < nfa["transitions"].size(); y++) {
            //als gegeven transitie en gegeven alphabet overeenkomen dan voegen we die toe aan new state
            if(count(state.begin(), state.end(), nfa["transitions"][y]["from"]) && nfa["transitions"][y]["input"] == nfa["alphabet"][x])
                new_state.push_back(nfa["transitions"][y]["to"]);
        }

        //sort de state alfabetsich of numeriek
        sort(new_state.begin(), new_state.end());

        //zet vector om in string
        string name = "{" + new_state[0];
        for(int i = 1; i < new_state.size(); i++){
            name += ",";
            name += new_state[i];
        }

        name += "}";

        map<string, std::any> map = {{"name", name}, {"lol", true}};

        //voeg niewe state toe aan states
        dfa["states"].push_back(map);

        findStates(new_state);
    }
}

DFA NFA::toDFA(){
    dfa = {
            {"type", "DFA"},
            {"alphabet", nfa["alphabet"]}
    };

    for (int i = 0; i < nfa["states"].size(); i++){
        if (nfa["states"][i]["starting"] == true){
            dfa["states"] = {"", (nfa["states"][i])};
        }
    }

    findStates({dfa["states"][1]["name"]});

    ofstream file("DFA.json");
    file << dfa;
    return DFA("DFA.json");
}