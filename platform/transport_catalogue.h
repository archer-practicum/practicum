#pragma once
#include <unordered_map>
#include <string>
#include <deque>
#include <unordered_set>

struct Stop {
    std::string name;
    double latitude;
    double longitude;
};

struct Bus {
    std::string name;
    std::deque<const Stop *> stops;
    bool is_circular;
    std::string ToString() const;
};

class TransportCatalogue {

public:
    void AddStop(const Stop &stop);
    void AddBus(const Bus &bus);
    const Stop* GetStop(const std::string &stop_name) const;
    const Bus* GetBus(const std::string &bus_name) const;
    const std::string GetInfoBus(const std::string &bus_name) const;
    const std::unordered_set<const Bus *> * GetBusesPassingStop(const std::string &stop_name) const;
    void SetDistanceBetweenTwoStops(const std::string &stop_name1, const std::string &stop_name2, size_t distance);
    size_t GetDistanceBetweenTwoStops(const std::string &stop_name1, const std::string &stop_name2);
    size_t GetDistanceBetweenTwoStops(const Stop * stop1, const Stop * stop2);

private:
    using hasher = std::hash<std::string>;

    class hash_two_stops {
    public:
        size_t operator()(const std::pair<const Stop *, const Stop *> &p) const {    
            return _hasher(p.first) + _hasher(p.second) * 37 * 37;
        }
        private:
        std::hash<const void *> _hasher;
    };

    std::unordered_map<std::string, Stop, hasher> _stops;
    std::unordered_map<std::string, Bus, hasher> _buses;
    std::unordered_map<std::string, std::unordered_set<const Bus *>, hasher> _all_buses_passing_stop;
    std::unordered_map<std::pair<const Stop *, const Stop *>, size_t, hash_two_stops> _distance_between_two_stops;
};
