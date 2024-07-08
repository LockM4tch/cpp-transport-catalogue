#include <string>
#include <iostream>

#include "request_handler.h"
#include "transport_catalogue.h"


int main() { using namespace std;

    TransportCatalogue catalogue;
    RequestHandler handler(catalogue);
    handler.CreateDocumentFromJSON(cin);
    handler.GetRequestResults(cout);
}