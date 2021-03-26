#include "NFA.h"
#include "ENFA.h"

using namespace std;

int main() {
    NFA nfa("input-ssc2.json");
    nfa.toDFA().print();
    return 0;
}