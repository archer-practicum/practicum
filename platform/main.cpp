#include <iostream>
#include <string>
#include <sstream>
#include <deque>
#include <vector>
#include <cmath>
#include <locale>
#include <unordered_map>
#include "geo.h"
#include <numeric>
#include <iomanip>

struct Stop {
    std::string name;
    double latitude;
    double longitude;
};

struct HashString {
    
    size_t operator()(const std::string str) const {
        return hasher(str);
    }

    private:
    std::hash<std::string> hasher;    
};

struct Bus {
    std::string name;
    std::deque<const Stop*> stops;

    std::string ToString() const {
        using namespace std::string_literals;
        std::stringstream sstr;

        sstr << "Bus "s << name << ": "s;

        size_t size = stops.size();
        size_t count_stops, count_unique_stops;
        double distance = 0.0;
        if (!size) return sstr.str();

        for (size_t i = 0u; i < stops.size() - 1; ++i) {
            distance += ComputeDistance({stops.at(i)->latitude, stops.at(i)->longitude}, 
                                        {stops.at(i + 1)->latitude, stops.at(i + 1)->longitude});
        }

        if (stops.front() == stops.back()) {
            count_stops = size;
            count_unique_stops = size - 1;
        } else {
            count_stops = 2 * size - 1;
            count_unique_stops = size;
            distance *= 2;
        }
        
        sstr << count_stops << " stops on route, "s 
        << count_unique_stops << " unique stops, "s 
        << std::setprecision(6) << distance << " route lenght"s;

        return sstr.str();
    }
};

std::unordered_map<size_t, Stop> all_stops;
std::unordered_map<size_t, Bus> all_buses;

void read(std::istream &input) {
    std::string line;
    std::getline(input, line);
    int count_query =  stoi(line);
    std::vector<std::string> query_route;
    query_route.reserve(count_query);
    size_t pos_begin = 0;
    size_t pos_end = 0;
    HashString hasher;

    while (std::getline(input, line)) {
        pos_begin = line.find_first_not_of(' ');

        if (line[pos_begin] == 'B') {
            query_route.push_back(move(line));
            continue;
        }

        pos_begin = line.find_first_not_of(' ', pos_begin + 4); // stop - content 4 char
        pos_end = line.find_first_of(':', pos_begin);
        std::string stop_name = line.substr(pos_begin, pos_end - pos_begin);

        pos_begin = pos_end + 1;
        pos_end = line.find_first_of(',', pos_begin);
        std::string latitude = line.substr(pos_begin, pos_end - pos_begin);

        pos_begin = pos_end + 1;        
        std::string longitude = line.substr(pos_begin);

        all_stops.insert({hasher(stop_name), {std::move(stop_name), stod(latitude), stod(longitude)}});

    }

    for (const std::string &line : query_route) {
        pos_begin = line.find_first_not_of(' ');
        pos_begin = line.find_first_not_of(' ', pos_begin + 3); // bus - content 3 char
        pos_end = line.find_first_of(':', pos_begin);
        std::string bus_name = line.substr(pos_begin, pos_end - pos_begin);

        std::deque<const Stop*> stops;
        pos_begin = pos_end + 1;

        while (1) {
            pos_begin = line.find_first_not_of(' ', pos_begin); // begin stop name
            if(pos_begin == line.npos) break;
            size_t pos_delimiter = line.find_first_of(std::string("->"), pos_begin); // find delimiter
            pos_end = line.find_last_not_of(' ', pos_delimiter - 1) + 1;    // +1 - результат работы ф. указывает на последнюю букву имени остановки
            std::string stop_name = line.substr(pos_begin, pos_end - pos_begin); 
            stops.push_back(&all_stops.at(hasher(stop_name)));
            if (pos_delimiter == line.npos) break;
            pos_begin = pos_delimiter + 1;
        }
        
        all_buses.insert({hasher(bus_name), {std::move(bus_name), std::move(stops)}});
    }

}

int main() {

    // russian global console key
    setlocale(LC_ALL,"Russian");

    std::istringstream sstring{ 
                                " 12\n"
                                "Stop Tolstopaltsevo: 55.611087, 37.208290\n"
                                "Stop Marushkino:  55.595884 ,  37.209755 \n"
                                "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
                                "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
                                "Bus 1: Tolstopaltsevo\n"
                                "Bus 2: \n"
                                "Stop Rasskazovka: 55.632761, 37.333324\n"
                                "Stop Biryulyovo Zapadnoye: 55.574371, 37.651700\n"
                                "Stop Biryusinka: 55.581065, 37.648390\n"
                                "Stop Universam: 55.587655, 37.645687\n"
                                "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
                                "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n"
                                };
    read(sstring);

    for (const auto &[key, bus] : all_buses) {
        std::cout << bus.ToString() << std::endl;
    }
                                 
}