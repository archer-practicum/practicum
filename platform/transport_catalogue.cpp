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
    stops.insert({hasher(stop.name), std::move(stop)});
}

void TransportCatalogue::AddBus(const Bus &bus) {
    buses.insert({hasher(bus.name), std::move(bus)});
}

const Stop* TransportCatalogue::GetStop(const std::string &name) const {
    return &stops.at(hasher(name));
}

const Bus* TransportCatalogue::GetBus(const std::string &name) const {
    return &buses.at(hasher(name));
}

const std::string TransportCatalogue::GetInfoBus(const std::string &name) const {
    using namespace std::string_literals;
    size_t hash = hasher(name);
    if (buses.count(hash)) {
        return buses.at(hash).ToString();
    } else {
        return "Bus " + name + ": not found"s;
    }
}