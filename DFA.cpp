#include "DFA.h"
#include <iostream>
#include <fstream>
#include "json.hpp"

using namespace std;

using json = nlohmann::json;

DFA::DFA(string p) {
    path = p;
    ifstream input(path);
    input >> dfa;
}

DFA::DFA(const DFA dfa1, const DFA dfa2, bool doorsnede) {
    path1 = dfa1.path;
    path2 = dfa2.path;
    d = doorsnede;
    product(string(path1), string(path2));
}


//kijkt of een string geaccepteerd wordt
bool DFA::accepts(string numbers) {
    string cur_node;

    for (int i = 0; i < dfa["states"].size(); i++) {
        if (dfa["states"][i]["starting"] == true)
            cur_node = dfa["states"][i]["name"];
    }

    for (int i = 0; i < numbers.size(); i++) {
        char num = numbers[i];
        for (int i = 0; i < dfa["transitions"].size(); i++) {
            string numb = dfa["transitions"][i]["input"];
            numb.erase(remove(numb.begin(), numb.end(), '\"'), numb.end());

            if (dfa["transitions"][i]["from"] == cur_node && numb[0] == num) {
                cur_node = dfa["transitions"][i]["to"];
                break;
            }
        }
    }

    for (int i = 0; i < dfa["states"].size(); i++) {
        if (dfa["states"][i]["name"] == cur_node)
            return dfa["states"][i]["accepting"];
    }
    return false;
}

//print de dfa
void DFA::print() {
    ifstream input(path);
    input >> dfa;
    cout << dfa.dump(4) << endl;
}

//zet een vector om in een string
string DFA::vecToString(vector<string> vec) {
    return "(" + vec[0] + "," + vec[1] + ")";
}

//checkt of een state accepterend of startend is
pair<bool, bool> DFA::check(vector<string> state) {
    vector<bool> bools = {false, false, false, false};
    pair<bool, bool> pair;

    for (int i = 0; i < dfa1["states"].size(); i++) {
        if (dfa1["states"][i]["name"] == state[0]) {
            if (dfa1["states"][i]["starting"])
                bools[0] = true;

            if (dfa1["states"][i]["accepting"])
                bools[2] = true;
        }
    }

    for (int i = 0; i < dfa2["states"].size(); i++) {
        if (dfa2["states"][i]["name"] == state[1]) {
            if (dfa2["states"][i]["starting"])
                bools[1] = true;

            if (dfa1["states"][i]["accepting"])
                bools[3] = true;
        }
    }

    pair.first = bools[0] && bools[1];

    if (d) {
        pair.second = bools[2] && bools[3];
    } else {
        pair.second = bools[2] || bools[3];
    }

    return pair;
}

//voegt een state toe aan de dfa
void DFA::addStates() {
    pair<bool, bool> c;

    for (auto const &state : states) {
        c = check(state);
        dfa["states"].push_back(
                {{"name",      vecToString(state)},
                 {"starting",  c.first},
                 {"accepting", c.second}});
    }
}

//voegt een transitie toe aan de dfa
void DFA::addTransition(string from, string to, string input) {
    dfa["transitions"].push_back(
            {{"from",  from},
             {"to",    to},
             {"input", input}});
}

//vindt de start state
string DFA::findStart(json d) {
    for (int i = 0; i < d["states"].size(); i++) {
        if (d["states"][i]["starting"])
            return d["states"][i]["name"];
    }
    return "";
}

//loopt over transities en geeft "to" terug voor een state en input
string DFA::stateLoop(json d, string state, string input) {
    for (int i = 0; i < d["transitions"].size(); i++) {
        if (d["transitions"][i]["from"] == state && d["transitions"][i]["input"] == input)
            return d["transitions"][i]["to"];
    }
    return "";
}

//vindt alle transities recursief
void DFA::findTransition(vector<string> state) {
    vector<vector<string>> new_states;

    if (states.find(state) != states.end())
        return;
    states.insert(state);

    for (auto const &alph : dfa["alphabet"]) {
        vector<string> new_state;

        new_state.push_back(stateLoop(dfa1, state[0], alph));
        new_state.push_back(stateLoop(dfa2, state[1], alph));

        addTransition(vecToString(state), vecToString(new_state), alph);
        new_states.push_back(new_state);
    }

    for (auto const &s : new_states) {
        findTransition(s);
    }
}

void DFA::product(const string &d1, const string &d2) {
    ifstream i1(d1);
    i1 >> dfa1;
    ifstream i2(d2);
    i2 >> dfa2;

    dfa = {
            {"type",     "DFA"},
            {"alphabet", dfa1["alphabet"]}
    };

    dfa["transitions"] = {"", ""};
    dfa["states"] = {"", ""};

    vector<string> start;
    start.push_back(findStart(dfa1));
    start.push_back(findStart(dfa2));

    findTransition(start);

    addStates();

    dfa["transitions"].erase(dfa["transitions"].begin());
    dfa["transitions"].erase(dfa["transitions"].begin());
    dfa["states"].erase(dfa["states"].begin());
    dfa["states"].erase(dfa["states"].begin());

    ofstream file("product.json");
    file << dfa;
    path = "product.json";
}