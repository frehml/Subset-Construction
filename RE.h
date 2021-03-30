//
// Created by frehml on 28/03/2021.
//

#ifndef SSC_RE_H
#define SSC_RE_H

#include "ENFA.h"
#include <string>
#include <set>

using namespace std;

class RE {
public:
    string regex;
    string eps;
    json renfa;
    vector<int> states = {0};
    set<string> alph;

    struct block {
        int start;
        int end;
        string expression;
    };

    vector<block> transitions;

    RE(string r, char e);

    ENFA toENFA();
    void addTransition(int from, int to, string input);
    void sum(block* R, block* S);
    void concat(block* R, block* S);
    void kleene(block* R);
    void findTransitions(block &a);
    pair<block, block> breakUp(block a);
    static string removeBracket(string expression);
    static bool noBrackets(string const &expression);
    static int calculateOperations(string const &expression);
    void doOperation(block* R, block* S, string const &operation);
    static void findStartEnd(block* a, int* startPoint, int* endpoint);
    static void findExpressions(int startPoint, int endPoint, string* expression1, string* expression2, string* operation, block a);
    static void buildBlock(block* x, block* y,block const &a,string const &expression1,string const &expression2, string const &operation);
    void buildTransitions();
    void buildStates(int end);
    void addState(int name, bool accepting, bool starting);
    void buildAlph();
    static bool noOperators(string const &expression);
};


#endif //SSC_RE_H
