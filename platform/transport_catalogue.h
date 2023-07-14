#pragma once
#include <unordered_map>
#include <string>
#include <sstream>
#include <deque>
#include "geo.h"
#include <iomanip>

struct Stop {
    std::string name;
    double latitude;
    double longitude;
};

struct Bus {
    std::string name;
    std::deque<const Stop*> stops;
    size_t count_stops = 0;
    size_t count_unique_stops = 0;
    double distance = 0;

    std::string ToString() const {
        using namespace std::string_literals;
        std::stringstream sstr;

        sstr << "Bus "s << name << ": "s        
        << count_stops << " stops on route, "s 
        << count_unique_stops << " unique stops, "s 
        << std::setprecision(6) << distance << " route lenght"s;

        return sstr.str();
    }
};

class TransportCatalogue {

    void AddStop(const Stop &&stop) {
        stops.insert({hasher(stop.name), stop});
    }

    void AddBus(const Bus &&bus) {
        size_t size = bus.stops.size();

        for (size_t i = 0u; i < bus.stops.size() - 1; ++i) {
            bus.distance += ComputeDistance({bus.stops.at(i)->latitude, bus.stops.at(i)->longitude}, 
                                        {bus.stops.at(i + 1)->latitude, bus.stops.at(i + 1)->longitude});
        }

        if (bus.stops.front() == bus.stops.back()) {
            bus.count_stops = size;
            bus.count_unique_stops = size - 1;
        } else {
            bus.count_stops = 2 * size - 1;
            bus.count_unique_stops = size;
            bus.distance *= 2;
        }

        buses.insert({hasher(bus.name), bus});
    }

    const Stop* GetStop(size_t hash) const {
        return &stops.at(hash);
    }

    const Bus* GetBus(size_t hash) const {
        return &buses.at(hash);
    }

    private:
    std::unordered_map<size_t, Stop> stops;
    std::unordered_map<size_t, Bus> buses;
    std::hash<std::string> hasher;
};