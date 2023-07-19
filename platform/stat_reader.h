#pragma once
#include <iostream>
#include <deque>
#include <string>
#include "transport_catalogue.h"
#include "input_reader.h"

void DatabaseQueries(const TransportCatalogue &catalogue, std::istream &input);

std::string GetInfoBus(const TransportCatalogue &catalogue, const std::string &str);

std::string GetInfoStop(const TransportCatalogue &catalogue, const std::string &str);


