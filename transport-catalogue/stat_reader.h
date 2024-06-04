#pragma once

#include <iosfwd>
#include <iostream>
#include <string_view>

#include "input_reader.h"
#include "transport_catalogue.h"

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
    std::ostream& output);

void ExecuteStatRequests(std::istream& in, std::ostream &out, TransportCatalogue& tansport_catalogue);
