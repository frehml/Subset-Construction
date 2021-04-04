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

    for (auto const &st : new_states) {
        findTransition(st);
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
    file.close();
    path = "product.json";
}

//verwijdert een state uit de transitions vector
void DFA::removeState(string const &state, string const &addative) {
    vector<transition> trans;
    for (auto const &transition1 : transitions.back()) {
        transition tran;

        if (transition1.from != state && transition1.to != state)
            trans.push_back(transition1);

        if(transition1.from != state ^ transition1.to != state){
            vector<transition> options;
            for (auto const &transition2 : transitions.back()) {
                if (&transition1 != &transition2) {
                    if (transition1.from != state && transition1.to == state && transition2.from == state && transition2.to != state) {
                        tran.from = transition1.from;
                        tran.to = transition2.to;
                        tran.expression = transition1.expression + addative + transition2.expression;
                        trans.push_back(tran);
                    }
                }
            }
        }
    }
    transitions.push_back(trans);
}

//for testing purposes it prints the last vector in transitions
void DFA::printTransitions(){
    for(auto const &transition : transitions.back()){
        cout << "from: " << transition.from << endl;
        cout << "to: " << transition.to << endl;
        cout << "expression: " << transition.expression << endl;
        cout << endl;
    }
    cout << endl;
}

//vindt de start transitions en voegt de transition blocks toe aan transitions
void DFA::startTransitions() {
    vector<transition> trans;
    for (auto tran : dfa["transitions"]) {
        transition t;
        t.from = tran["from"];
        t.to = tran["to"];
        t.expression = tran["input"];
        if (t.to == t.from)
            t.expression += "*";
        trans.push_back(t);
    }
    transitions.push_back(trans);
}

//zoekt alle niet accepterende en startende states
void DFA::getStates(){
    for(auto state : dfa["states"]){
        if(state["accepting"])
            accepting[state["name"]]= true;
        else
            accepting[state["name"]]= false;

        if(state["starting"])
            starting[state["name"]]= true;
        else
            starting[state["name"]]= false;


        if(!state["accepting"] && !state["starting"])
            s.push_back(stoi(string(state["name"])));
    }
    sort(s.begin(), s.end());
}

void DFA::sumEquals(){
    vector<transition> t = transitions.back();
    vector<transition> trans;
    for(int x = 0; x <= t.size(); x++){
        transition transition;
        string expression = t[x].expression;
        for(int y = 0; y <= t.size(); y++){
            if(y > x){
                if(t[x].from == t[y].from && t[x].to == t[y].to){
                    expression += "+"+t[y].expression;
                }
            }
        }
        transition = {t[x].from, t[x].to, expression};
        if(!trans.empty()){
            if(transition.from != trans.back().from || transition.to != trans.back().to)
                trans.push_back(transition);
        }
        else
            trans.push_back(transition);
    }
    transitions.push_back(trans);
}

/*
void DFA::sumEquals(){
    vector<transition> t = transitions.back();
    vector<transition> trans;
    for(int x = 0; x <= t.size(); x++){
        transition transition;
        string expression = t[x].expression;
        for(int y = 0; y <= t.size(); y++){
            if(y > x){
                if(t[x].from == t[y].from && t[x].to == t[y].to){
                    expression += "+"+t[y].expression;
                }
            }
        }
        transition = {t[x].from, t[x].to, expression};
        if(!trans.empty()){
            if(transition.from != trans.back().from || transition.to != trans.back().to)
                trans.push_back(transition);
        }
        else
            trans.push_back(transition);
    }
    transitions.push_back(trans);
}
*/

string DFA::format(){
    string R, S, U, T;
    for(auto const &transition : transitions.back()){
        if(starting[transition.from] && starting[transition.to])
            R = transition.expression;
        else if (starting[transition.from] && accepting[transition.to])
            S = transition.expression;
        else if (accepting[transition.from] && accepting[transition.to])
            U = transition.expression;
        else if (starting[transition.to] && accepting[transition.from])
            T = transition.expression;
    }
    if(R.empty() && T.empty())
        return S+U;
    return R+S+U+T+S+U;
}

string DFA::findAddative(string const &state){
    string addative;
    for(auto const &transition : transitions.back()){
        if(transition.from == state && transition.to == state)
            addative += transition.expression + "+";
    }
    if(!addative.empty())
        addative = "(" + (addative.substr(0, addative.size()-2)) + ")" + "*";
    return addative;
}

RE DFA::toRE() {
    startTransitions();
    getStates();
    for(auto state : s){
        string st = to_string(state);
        removeState(st, findAddative(st));
        cout << "remove state " << state << endl << endl;
        printTransitions();
    }
    sumEquals();
    printTransitions();
    RE re(format(), 'e');
    return re;
}

//      (f(d)*f+d(d+f(d)*f))(f+d)*