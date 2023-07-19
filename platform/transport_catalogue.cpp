#include "transport_catalogue.h"
#include <sstream>
#include <set>

const Stop* TransportCatalogue::AddStop(const Stop &stop) {
    _all_buses_passing_stop[stop.name];
    const auto &[it, b] = _stops.insert({stop.name, std::move(stop)});
    return &it->second;
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

const std::unordered_set<const Bus *> * TransportCatalogue::GetBusesPassingStop(const std::string &stop_name) const {
    return _all_buses_passing_stop.count(stop_name) ? &_all_buses_passing_stop.at(stop_name) : nullptr;
}

void TransportCatalogue::SetDistanceBetweenTwoStops(const std::string &stop_name1, const std::string &stop_name2, size_t distance) {
    const Stop * stop1 = GetStop(stop_name1);
    const Stop * stop2 = GetStop(stop_name2);
    SetDistanceBetweenTwoStops(stop1, stop2, distance);
}

void TransportCatalogue::SetDistanceBetweenTwoStops(const Stop * stop1, const Stop * stop2, size_t distance) {
    _distance_between_two_stops[{stop1, stop2}] = distance;
}

size_t TransportCatalogue::GetDistanceBetweenTwoStops(const std::string &stop_name1, const std::string &stop_name2) {
    const Stop * stop1 = GetStop(stop_name1);
    const Stop * stop2 = GetStop(stop_name2);
    return GetDistanceBetweenTwoStops(stop1, stop2);
}

size_t TransportCatalogue::GetDistanceBetweenTwoStops(const Stop * stop1, const Stop * stop2) {
    return _distance_between_two_stops.count({stop1, stop2}) ? _distance_between_two_stops.at({stop1, stop2}) : _distance_between_two_stops.at({stop2, stop1});
}