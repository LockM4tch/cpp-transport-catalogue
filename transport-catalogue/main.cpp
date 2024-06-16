#include <iostream>
#include <string>

#include "request_handler.h"
#include "transport_catalogue.h"

using namespace std;

int main() {
    TransportCatalogue catalogue;
    RequestHandler handler(catalogue);
    handler.CreateDocumentFromJSON(std::cin);
    handler.GetRequestResults(std::cout);
}