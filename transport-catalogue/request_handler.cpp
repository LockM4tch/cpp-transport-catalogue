#include "request_handler.h"

RequestHandler::RequestHandler(TransportCatalogue& db) :db_(db), renderer_(), document_(nullptr) {
}

void RequestHandler::CreateDocumentFromJSON(std::istream& in) {
    document_ = std::make_unique<json::Document>(std::move(json::Load(in)));
}
void RequestHandler::GetRequestResults(std::ostream& out) {
    JsonReader jr(db_, renderer_);
    jr.ProcessRequest(*document_, out);
}
