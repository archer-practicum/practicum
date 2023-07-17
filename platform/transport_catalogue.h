#pragma once
#include <unordered_map>
#include <string>
#include <deque>

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
    const Stop* GetStop(const std::string &name) const;
    const Bus* GetBus(const std::string &name) const;
    const std::string GetInfoBus(const std::string &name) const;

    private:
    std::unordered_map<size_t, Stop> stops;
    std::unordered_map<size_t, Bus> buses;
    std::hash<std::string> hasher;
};
