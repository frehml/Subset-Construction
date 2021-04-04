#include "DFA.h"
#include <iostream>
#include <fstream>
#include "json.hpp"

using namespace std;

using json = nlohmann::json;

/**
 * constructor
 * @param p
 */
DFA::DFA(string p) {
    path = p;
    ifstream input(path);
    input >> dfa;
}

/**
 * contructor
 * @param dfa1
 * @param dfa2
 * @param doorsnede
 */
DFA::DFA(const DFA dfa1, const DFA dfa2, bool doorsnede) {
    path1 = dfa1.path;
    path2 = dfa2.path;
    d = doorsnede;
    product(string(path1), string(path2));
}

/**
 *
 * @param numbers
 * @return
 */
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

/**
 *
 */
//print de dfa
void DFA::print() {
    ifstream input(path);
    input >> dfa;
    cout << dfa.dump(4) << endl;
}

/**
 *
 * @param vec
 * @return
 */
//zet een vector om in een string
string DFA::vecToString(vector<string> vec) {
    return "(" + vec[0] + "," + vec[1] + ")";
}

/**
 *
 * @param state
 * @return
 */
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

/**
 *
 * @param from
 * @param to
 * @param input
 */
//voegt een transitie toe aan de dfa
void DFA::addTransition(string from, string to, string input) {
    dfa["transitions"].push_back(
            {{"from",  from},
             {"to",    to},
             {"input", input}});
}

/**
 *
 * @param d
 * @return
 */
//vindt de start state
string DFA::findStart(json d) {
    for (int i = 0; i < d["states"].size(); i++) {
        if (d["states"][i]["starting"])
            return d["states"][i]["name"];
    }
    return "";
}

/**
 *
 * @param d
 * @param state
 * @param input
 * @return
 */
//loopt over transities en geeft "to" terug voor een state en input
string DFA::stateLoop(json d, string state, string input) {
    for (int i = 0; i < d["transitions"].size(); i++) {
        if (d["transitions"][i]["from"] == state && d["transitions"][i]["input"] == input)
            return d["transitions"][i]["to"];
    }
    return "";
}

/**
 *
 * @param state
 */
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

/**
 * maakt de product DFA
 * @param d1
 * @param d2
 */
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

/**
 *
 * @param state
 * @param addative
 */
//verwijdert een state uit de transitions vector
void DFA::removeState(string const &state, string const &addative) {
    vector<transition> trans;
    for (auto const &transition1 : transitions.back()) {
        transition tran;

        if (transition1.from != state && transition1.to != state)
            trans.push_back(transition1);

        if(transition1.from != state ^ transition1.to != state){
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
        trans.push_back(t);
    }
    transitions.push_back(trans);
}

//zoekt alle niet accepterende en startende states
void DFA::getStates(){
    vector<int> temp;
    for(auto state : dfa["states"]){
        stateNames.push_back(state["name"]);
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
    sort(stateNames.begin(), stateNames.end());
    sort(s.begin(), s.end());
}


void DFA::sumEquals(){
    vector<transition> trans;
    for(auto const &transition1 : transitions.back()){
        transition tran = transition1;
        for(auto const &transition2 : transitions.back()){
            if(transition1.from == transition2.from && transition1.to == transition2.to && transition1.expression != transition2.expression){
                tran.from = transition1.from;
                tran.to = transition1.to;
                if(transition1.expression.size() <= transition2.expression.size())
                    tran.expression = transition1.expression + "+" + transition2.expression;
                else
                    tran.expression = transition2.expression + "+" + transition1.expression;
            }
        }
        if(tran.expression != transition1.expression)
            tran.expression = "(" + tran.expression + ")";

        if(!trans.empty()){
            bool check = true;
            for(auto const &t : trans){
                if(t.from == tran.from && t.to == tran.to)
                    check = false;
            }
            if(check){
                trans.push_back(tran);
            }
        }
        else
            trans.push_back(tran);
    }
    transitions.push_back(trans);
}

/**
 * zet de laatste transitie structs om in een reguliere expressie
 * @return
 */
string DFA::format(){
    string expression;
    for(auto const &name : stateNames){
        string R, S, U, T;
        if(!starting[name] && !sortedTransitions[name].empty()){
            for(auto const &transition : sortedTransitions[name]){
                if(transition.to == transition.from)
                    U = transition.expression;
                else if (starting[transition.to])
                    T = transition.expression;
            }

            for(auto const &transition : sortedTransitions["start"]){
                if(transition.from == transition.to)
                    R = transition.expression;
                else if (transition.to == name){
                    S = transition.expression;
                }
            }
            if(!expression.empty())
                expression += "+";

            if(R.empty() && T.empty())
                expression += S+U+"*";
            else if (R.empty())
                expression += "("+S+U+T+")"+"*"+S+U;
            else
                expression += "("+R+"+"+S+U+T+")"+"*"+S+U;
        }
    }
    return expression;
}

/**
 * zoekt of een node een transitie op zichzelf heeft
 * @param state
 * @return
 */
string DFA::findAddative(string const &state){
    string addative;
    for(auto const &transition : transitions.back()){
        if(transition.from == state && transition.to == state)
            addative = transition.expression;
    }
    if(!addative.empty())
        addative = "(" + addative +  ")" + "*";
    return addative;
}

RE DFA::toRE() {
    startTransitions();
    getStates();
    sumEquals();
    for(auto state : s){
        string st = to_string(state);
        removeState(st, findAddative(st));
        sumEquals();
    }

    for(auto const &transition : transitions.back()){
        if(starting[transition.from])
            sortedTransitions["start"].push_back(transition);
        else
            sortedTransitions[transition.from].push_back(transition);
    }

    RE re(format(), 'e');
    return re;
}
//      (f(d)*f+d(d+f(d)*f))(f+d)*

//zoekt de start kruisjes (waar een accepterend state is)
void DFA::startingX(){
    for(auto const &name1 : stateNames){
        for(auto const &name2 : stateNames){
            if(accepting[name1] ^ accepting[name2]){
                table[{name1, name2}] = true;
            }
            else
                table[{name1, name2}] = false;
        }
    }
}

//checkt of string op een kruisje valt
bool DFA::acceptCheck(string name1, string name2){
    vector<vector<string>> possibles;
    for(auto const &alph : dfa["alphabet"]){
        vector<string> posib(2);
        for(auto tran : dfa["transitions"]){
            if(tran["input"] == alph && tran["from"] == name1)
                posib[0] = tran["to"];
            else if (tran["input"] == alph && tran["from"] == name2)
                posib[1] = tran["to"];
        }
        possibles.push_back(posib);
    }

    for(auto const &p : possibles){
        if(table[p])
            return true;
    }
    return false;
}

//zoekt nieuwe kruisjes met eerder verkregen kruisjes
void DFA::recursiveX(){
    for(auto const &name1 : stateNames){
        for(auto const &name2 : stateNames){
            if(acceptCheck(name1, name2)){
                table[{name1, name2}] = true;
            }
        }
    }
}
/**
 *
 * @param vec
 */
//maakt de states van de nieuwe DFA
void DFA::makeStates(vector<vector<string>> const &vec){
    for(auto const &v : vec){
        bool start = false;
        bool accept = false;
        string name = "{";
        for(auto const &elem : v){
            if(starting[elem])
                start = true;

            if(accepting[elem])
                accept = true;

            name += elem + ", ";
        }
        name = name.substr(0, name.size()-2);
        name += "}";
        new_dfa["states"].push_back({{"name", name},
                                     {"starting", start},
                                     {"accepting", accept}});
    }
}

/**
 * vector naar string
 * @param vec
 * @return
 */
string DFA::vToString(vector<string> const &vec){
    string name = "{";
    for(auto const &elem : vec){
        name += elem + ", ";
    }
    name = name.substr(0, name.size()-2);
    name += "}";
    return name;
}

/**
 * checkt of to niet vervangen moet worden door een grotere node
 * @param to
 * @param st
 * @return
 */
vector<string> DFA::toCheck(vector<string> to, vector<vector<string>> const &st){
    for(auto state : st){
        bool check = true;
        for(auto const &t : to){
            if (std::find(state.begin(), state.end(), t) == state.end())
                check = false;
        }
        if(check)
            return state;
    }
    return to;
}

/**
 * maakt de transities van de nieuwe DFA
 * @param st
 */
void DFA::makeTransitions(vector<vector<string>> const &st){
    for(auto state : st){
        for(auto const &alph : dfa["alphabet"]){
            vector<string> to;
            for(auto transition : dfa["transitions"]){
                if(count(state.begin(), state.end(), transition["from"]) && alph == transition["input"]){
                    to.push_back(transition["to"]);
                }
            }
            to = toCheck(to, st);

            new_dfa["transitions"].push_back({{"from", vToString(state)},
                                         {"to", vToString(to)},
                                         {"input", alph}});
        }
    }
}

/**
 * vindt de states vanuit de TFA table
 */
void DFA::findStates(){
    set<string> check;
    vector<vector<string>> new_states;
    vector<vector<string>> old_states;
    for(auto const &name1 : stateNames){
        for(auto const &name2 : stateNames){
            if(name1 < name2 && !table[{name1, name2}]){
                check.insert(name1);
                check.insert(name2);
                new_states.push_back({name1, name2});
            }
            else if (name1 < name2 && table[{name1, name2}]){
                old_states.push_back({name1});
            }
        }
    }
    old_states.erase(unique(old_states.begin(), old_states.end()), old_states.end());

    if(check.size() <= new_states.size()){
        vector<string> new_state;
        for(auto const &elem : check){
            new_state.push_back(elem);
        }
        new_states = {};
        new_states.push_back(new_state);
        sort(new_states.begin(), new_states.end());
    }

    makeStates(new_states);
    makeStates(old_states);
    new_states.insert( new_states.end(), old_states.begin(), old_states.end());
    makeTransitions(new_states);
}

/**
 * maakt de DFA van de TFA
 * @return
 */
string DFA::createDFA(){
    new_dfa = {
            {"type",     "DFA"},
            {"alphabet", dfa["alphabet"]},
            {"states", {"", ""}},
            {"transitions", {"", ""}}
    };

    findStates();

    new_dfa["states"].erase(new_dfa["states"].begin());
    new_dfa["states"].erase(new_dfa["states"].begin());
    new_dfa["transitions"].erase(new_dfa["transitions"].begin());
    new_dfa["transitions"].erase(new_dfa["transitions"].begin());

    ofstream file("TableFillingDFA.json");
    file << new_dfa;
    file.close();
    return "TableFillingDFA.json";
}

/**
 * minimaliseert de DFA
 * @return
 */
DFA DFA::minimize(){
    getStates();
    startingX();
    map<vector<string>, bool> prevTable;
    while(prevTable != table){
        prevTable = table;
        recursiveX();
    }
    DFA d(createDFA());
    return d;
}

/**
 * print de TFA tabel
 */
void DFA::printTable(){
    for(int x = 1; x < stateNames.size(); x++){
        cout << stateNames[x];
        for(int y = 0; y < x; y++){
            string first = stateNames[x];
            string second = stateNames[y];
            if(table[{first, second}])
                cout << "\t" << "X";
            else
                cout << "\t" << "-";
        }
        cout << endl;
    }
    cout << "\t";
    for(int i = 0; i < stateNames.size()-1; i++){cout << stateNames[i] << "\t";}
    cout << endl;
}