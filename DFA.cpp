#include "DFA.h"
#include <iostream>
#include <fstream>
#include "json.hpp"
using namespace std;

using json = nlohmann::json;

DFA::DFA(string p){
    path = p;
}

bool DFA::accepts(string numbers){
    ifstream input(path);
    input >> dfa;

    string cur_node;

    for (int i = 0; i < dfa["states"].size(); i++){
        if (dfa["states"][i]["starting"] == true)
            cur_node =dfa["states"][i]["name"];
    }

    for (int i = 0; i < numbers.size(); i++) {
        char num = numbers[i];
        for (int i = 0; i < dfa["transitions"].size(); i++){
            string numb = dfa["transitions"][i]["input"];
            numb.erase(remove( numb.begin(), numb.end(), '\"' ),numb.end());

            if (dfa["transitions"][i]["from"] == cur_node && numb[0] == num){
                cur_node = dfa["transitions"][i]["to"];
                break;
            }
        }
    }

    for (int i = 0; i < dfa["states"].size(); i++){
        if (dfa["states"][i]["name"] == cur_node)
            return dfa["states"][i]["accepting"];
    }
}

void DFA::print(){
    ifstream input(path);
    input >> dfa;
    cout << dfa.dump(4) << endl;
}