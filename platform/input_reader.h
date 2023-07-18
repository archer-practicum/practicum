#pragma once

#include "transport_catalogue.h"
#include <iostream>
#include <deque>
#include <string>
#include <string_view>

struct TypesQueries {
    std::deque<std::string> buses;
    std::deque<std::string> stops;
};


void DatabaseFilling(TransportCatalogue &catalogue, std::istream &input);

TypesQueries ReadQueries(std::istream &input);

Bus ReadBus(const std::string &query, TransportCatalogue &catalogue);

Stop ReadStop(const std::string &query);

std::string_view SkipSpaceReadUpToSign(const std::string &str, char sign, size_t &pos_begin, size_t pos_end);