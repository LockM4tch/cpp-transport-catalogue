#pragma once
#include "transport_catalogue.h"
#include "json.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "domain.h"
#include <iostream>
#include <memory>

 class RequestHandler {
 public:
     RequestHandler(TransportCatalogue& db);

     void CreateDocumentFromJSON(std::istream& in);
     void GetRequestResults(std::ostream& out);

 private:
    TransportCatalogue& db_;
    MapRenderer renderer_;
    std::unique_ptr<json::Document> document_;
 };
