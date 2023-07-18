#include "transport_catalogue.h"
#include "geo.h"
#include <iomanip>
#include <sstream>
#include <set>

std::string Bus::ToString() const {
    using namespace std::string_literals;
    std::stringstream sstr;
    size_t count_stops = 0;
    std::set<std::string> unique_stops;
    double distance = 0;

    for (size_t i = 0u; i < stops.size() - 1; ++i) {
        distance += ComputeDistance({stops.at(i)->latitude, stops.at(i)->longitude},
                                    {stops.at(i + 1)->latitude, stops.at(i + 1)->longitude});
    }
    
    if (is_circular) {
        count_stops = stops.size();
    } else {
        count_stops = 2 * stops.size() - 1;        
        distance *= 2;
    }

    for (const Stop *stop : stops) {
        unique_stops.insert(stop->name);
    }

    sstr << "Bus "s << name << ": "s        
    << count_stops << " stops on route, "s 
    << unique_stops.size() << " unique stops, "s 
    << std::setprecision(6) << distance << " route length"s;

    return sstr.str();
}

void TransportCatalogue::AddStop(const Stop &stop) {
    _all_buses_passing_stop[stop.name];
    _stops.insert({stop.name, std::move(stop)});
}

void TransportCatalogue::AddBus(const Bus &bus) {    
    const auto&[it, b] = _buses.insert({bus.name, std::move(bus)});
    for (const Stop* stop : it->second.stops) {
        _all_buses_passing_stop.at(stop->name).insert(&it->second);
    }
}

const Stop* TransportCatalogue::GetStop(const std::string &stop_name) const {
    return _stops.count(stop_name) ? &_stops.at(stop_name) : nullptr;
}

const Bus* TransportCatalogue::GetBus(const std::string &bus_name) const {
    return _buses.count(bus_name) ? &_buses.at(bus_name) : nullptr;
}

const std::string TransportCatalogue::GetInfoBus(const std::string &bus_name) const {
    using namespace std::string_literals;
    return _buses.count(bus_name) ? _buses.at(bus_name).ToString() : "Bus "s + bus_name + ": not found"s;
}

const std::unordered_set<const Bus *> * TransportCatalogue::GetBusesPassingStop(const std::string &stop_name) const {
    return _all_buses_passing_stop.count(stop_name) ? &_all_buses_passing_stop.at(stop_name) : nullptr;
}

void TransportCatalogue::SetDistanceBetweenTwoStops(const std::string &stop_name1, const std::string &stop_name2, size_t distance) {
    _distance_between_two_stops[{GetStop(stop_name1), GetStop(stop_name2)}] = distance;
}

size_t TransportCatalogue::GetDistanceBetweenTwoStops(const std::string &stop_name1, const std::string &stop_name2) {
    const Stop * stop1 = GetStop(stop_name1);
    const Stop * stop2 = GetStop(stop_name2);
    return GetDistanceBetweenTwoStops(stop1, stop2);
}

size_t TransportCatalogue::GetDistanceBetweenTwoStops(const Stop * stop1, const Stop * stop2) {
    return _distance_between_two_stops.count({stop1, stop2}) ? _distance_between_two_stops.at({stop1, stop2}) : _distance_between_two_stops.at({stop2, stop1});
}