#include "DFA.h"
#include <iostream>
#include <fstream>
#include "json.hpp"
using namespace std;

using json = nlohmann::json;

bool DFA::accepts(string numbers){
    ifstream input(path);
    json j;
    input >> j;

    string cur_node;

    for (int i = 0; i < j["states"].size(); i++){
        if (j["states"][i]["starting"] == true)
            cur_node =j["states"][i]["name"];
    }

    for (int i = 0; i < numbers.size(); i++) {
        char num = numbers[i];
        for (int i = 0; i < j["transitions"].size(); i++){
            string numb = j["transitions"][i]["input"];
            numb.erase(remove( numb.begin(), numb.end(), '\"' ),numb.end());

            if (j["transitions"][i]["from"] == cur_node && numb[0] == num){
                cur_node = j["transitions"][i]["to"];
                break;
            }
        }
    }

    for (int i = 0; i < j["states"].size(); i++){
        if (j["states"][i]["name"] == cur_node)
            return j["states"][i]["accepting"];
    }
}

void DFA::print(){
    cout << "nuffin";
}