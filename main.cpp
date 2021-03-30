#include "RE.h"
#include "ENFA.h"
#include "NFA.h"
#include <iostream>

using namespace std;

int main() {
    DFA dfa("input-state1.json");
    dfa.toRE().print();
    return 0;
}