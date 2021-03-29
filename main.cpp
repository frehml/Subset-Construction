#include "RE.h"
#include "ENFA.h"
#include <iostream>
using namespace std;

int main() {
    RE re("(kd+x+k)(i)*(k+i)",'x');
    ENFA enfa = re.toENFA();
    enfa.printStats();

    // geven true
    cout << boolalpha << enfa.accepts("kdk") << endl;
    cout << boolalpha << enfa.accepts("i") << endl;
    cout << boolalpha << enfa.accepts("kiiiiii") << endl;

    // geven false
    cout << boolalpha << enfa.accepts("kikk") << endl;
    cout << boolalpha << enfa.accepts("") << endl;
    cout << boolalpha << enfa.accepts("kdiiiiiiki") << endl;

    return 0;
}