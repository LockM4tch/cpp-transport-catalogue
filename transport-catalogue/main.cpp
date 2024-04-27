#include <iostream>
#include <string>

#include "input_reader.h"
#include "tests.h"
#include "stat_reader.h"

using namespace std;

int main() {

    test();

    TransportCatalogue catalogue;

    Fill_Catalogue(cin, catalogue);

    Request_Stat(cin, cout, catalogue);

}