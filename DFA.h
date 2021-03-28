#ifndef OPDRACH1_DFA_H
#define OPDRACH1_DFA_H

#include <string>
#include <iostream>
#include "json.hpp"
#include <set>
#include <map>

using json = nlohmann::json;
using namespace std;

class DFA {

public:
    string path;
    json dfa;
    string path1;
    string path2;
    json dfa1;
    json dfa2;
    bool d;

    map<string, vector<vector<string>>> transitions;

    set<vector<string>> states;

    void product(string dfa1, string dfa2);

    DFA(string p);
    DFA(DFA dfa1, DFA dfa2, bool doorsnede);

    void multiply(string from1, string to1, string from2, string to2, string input);
    void addTransition(string from, string to, string input);
    pair<bool, bool> check(vector<string> state);
    string vecToString(vector<string> vec);
    void addStates();
    bool accepts(string numbers);
    void print();
    string findStart(json d);
    string stateLoop(json d, string state, string input);
    void findTransition(vector<string> state);
};


#endif //OPDRACH1_DFA_H
