#include "stat_reader.h"
#include <string>
#include <deque>
#include <limits>
#include <sstream>
#include <set>
#include "geo.h"
#include <iomanip>

using namespace std::string_literals;

void DatabaseQueries(const TransportCatalogue &catalogue, std::istream &input) {
    
    const auto &queries = detail::ReadQueries(input);

    for (const std::string &query : queries) {
        std::string info;
        if (query.at(0) == 'B') {
            info = GetInfoBus(catalogue, query);
        } else if (query.at(0) == 'S') {
            info = GetInfoStop(catalogue, query);
        }
        std::cout << info << std::endl;
    }
}

std::string GetInfoBus(const TransportCatalogue &catalogue, const std::string &str) {
    size_t pos_begin = 3;  // bus - content 3 char
    pos_begin = str.find_first_not_of(' ', pos_begin);
    std::string bus_name = str.substr(pos_begin);

    const Bus * bus = catalogue.GetBus(bus_name);
    std::stringstream res;
    res << "Bus "s << bus_name << ": "s;

    if (!bus) return res.str() + "not found"s;    

    size_t count_stops = 0;
    std::set<std::string> unique_stops;
    double distance = 0;

    for (size_t i = 0u; i < bus->stops.size() - 1; ++i) {
        distance += ComputeDistance({bus->stops.at(i)->latitude, bus->stops.at(i)->longitude},
                                    {bus->stops.at(i + 1)->latitude, bus->stops.at(i + 1)->longitude});
    }
    
    if (bus->is_circular) {
        count_stops = bus->stops.size();
    } else {
        count_stops = 2 * bus->stops.size() - 1;        
        distance *= 2;
    }

    for (const Stop *stop : bus->stops) {
        unique_stops.insert(stop->name);
    }
    
    res << count_stops << " stops on route, "s 
    << unique_stops.size() << " unique stops, "s 
    << std::setprecision(6) << distance << " route length"s;

    return res.str();
}

std::string GetInfoStop(const TransportCatalogue &catalogue, const std::string &str) {
    size_t pos_begin = 4; // stop - content 4 char
    pos_begin = str.find_first_not_of(' ', pos_begin);
    std::string stop_name = str.substr(pos_begin);    
    const auto* buses_passing_stop = catalogue.GetBusesPassingStop(stop_name);
    std::stringstream res;
    res << "Stop "s << stop_name << ": "s;

    if (!buses_passing_stop) return res.str() + "not found"s;
    
    if (buses_passing_stop->size()) {
        res << "buses"s;
        std::set<std::string> stops;
        for (const Bus* stop : *buses_passing_stop) {
            stops.insert(stop->name);
        }
        for (const std::string &stop_name : stops) {
            res << " "s << stop_name;
        }
    } else {
        res << "no buses"s;
    }
    
    return res.str();
}