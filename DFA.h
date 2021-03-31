#ifndef OPDRACH1_DFA_H
#define OPDRACH1_DFA_H

#include "json.hpp"
#include "RE.h"
#include <string>
#include <iostream>
#include <set>
#include <map>

using json = nlohmann::json;
using namespace std;

class RE;

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

    void product(const string &d1, const string &d2);

    explicit DFA(string p);
    DFA(DFA dfa1, DFA dfa2, bool doorsnede);

    void addTransition(string from, string to, string input);
    pair<bool, bool> check(vector<string> state);
    static string vecToString(vector<string> vec);
    void addStates();
    bool accepts(string numbers);
    void print();
    static string findStart(json d);
    static string stateLoop(json d, string state, string input);
    void findTransition(vector<string> state);
    RE toRE();
};


#endif //OPDRACH1_DFA_H
