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
    std::deque<const Stop*> stops;
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
    const std::unordered_set<const Bus*>* GetBusesPassingStop(const std::string &stop_name) const;

    private:
    using hasher = std::hash<std::string>;
    std::unordered_map<std::string, Stop, hasher> _stops;
    std::unordered_map<std::string, Bus, hasher> _buses;
    std::unordered_map<std::string, std::unordered_set<const Bus*>, hasher> _all_buses_passing_stop;
    
};
