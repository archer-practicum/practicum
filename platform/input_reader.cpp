#include "input_reader.h"
#include <unordered_map>
#include <limits>

void DatabaseFilling(TransportCatalogue &catalogue, std::istream &input) {    
    using namespace detail;
    const auto &queries = ReadQueries(input);
    std::deque<std::string> bus_queries;
    std::unordered_map<const Stop *, std::string_view> stop_and_disnance_other_stops;

    for (const std::string &str : queries) {
        if ((str.at(0) == 'B')) { 
            bus_queries.push_back(std::move(str));
            continue;
        }
        
        size_t pos_begin = 0;
        const Stop* stop = catalogue.AddStop(ReadStop(str, pos_begin));
        if (pos_begin != std::string::npos) stop_and_disnance_other_stops[stop] = std::string_view{&str.at(pos_begin)};
    }

    for (const auto &[stop, str_v] : stop_and_disnance_other_stops) {
        size_t pos_begin = 0;
        while (pos_begin != std::string_view::npos) {
            const auto &[another_stop_name, distance] = ReadDistanceToAnotherStop(str_v, pos_begin);
            const Stop * another_stop = catalogue.GetStop(std::string(another_stop_name));
            catalogue.SetDistanceBetweenTwoStops(stop, another_stop, distance);
        }        
    }

    for (const std::string &str : bus_queries) {
        catalogue.AddBus(ReadBus(catalogue, str));
    }
}

namespace detail {

    using namespace std::string_literals;

    std::deque<std::string> ReadQueries(std::istream &input) {
        int count_queries;
        std::deque<std::string> res;

        while(1) {
            input >> count_queries;
    
            if(input.eof() || input.bad())
                return res;
            else if(input.fail()) {
                input.clear(); // unset failbit
                input.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // skip bad input
            } else {
                input.ignore(1);
                break;
            }
        }

        for (int i = 0; i < count_queries; ++i) {
            std::string line;
            std::getline(input, line);
            res.push_back(std::move(line));                
        }

        return res;
    }

    Bus ReadBus(const TransportCatalogue &catalogue, const std::string_view &str_v) {
        size_t pos_begin = 3; // bus - content 3 char    
        
        std::string_view bus_name = ReadUpToSigns(str_v, ":"s, pos_begin);
        std::deque<const Stop *> stops = ReadBusRoute(catalogue, str_v, pos_begin);
        bool is_circule = IsCircularRoute(str_v);

        return {std::string(bus_name), stops, is_circule};
    }

    Stop ReadStop(const std::string_view &str_v, size_t &pos_begin) {
        pos_begin = 4;   // stop - content 4 char

        std::string_view stop_name = ReadUpToSigns(str_v, ":"s, pos_begin);    
        double latitude, longitude;
        ReadNumber(str_v, ","s, pos_begin, latitude);
        ReadNumber(str_v, ","s, pos_begin, longitude);

        return {std::string(stop_name), latitude, longitude};
    }

    std::string_view ReadUpToSigns(const std::string_view &str_v, const std::string &signs, size_t &pos_begin) {
        pos_begin = str_v.find_first_not_of(' ', pos_begin);
        size_t pos_delimiter = str_v.find_first_of(signs, pos_begin);

        std::string_view res;
        if (pos_delimiter == std::string_view::npos) {
            size_t pos_end = str_v.at(str_v.size() - 1) == ' ' ? str_v.find_last_not_of(' ', str_v.size() - 1) + 1 : str_v.size(); // +1 - результат работы ф. указывает на последнюю букву
            res = {&str_v.at(pos_begin), pos_end - pos_begin};
            pos_begin = std::string_view::npos;
        } else {
            size_t pos_end = str_v.at(pos_delimiter - 1) == ' ' ? str_v.find_last_not_of(' ', pos_delimiter - 1) + 1 : pos_delimiter; // +1 - результат работы ф. указывает на последнюю букву     
            res = {&str_v.at(pos_begin), pos_end - pos_begin};
            pos_begin = pos_delimiter + 1;        
        }    
        return res;
    }

    std::deque<const Stop *> ReadBusRoute(const TransportCatalogue &catalogue, const std::string_view &str_v, size_t &pos_begin) {
        std::deque<const Stop *> res;
        while (pos_begin != std::string_view::npos) {
            std::string_view stop_name = ReadUpToSigns(str_v, "->"s, pos_begin);
            res.push_back(catalogue.GetStop(std::string(stop_name)));
        }
        return res;
    }

    bool IsCircularRoute(const std::string_view &str_v) {
        return str_v.at(str_v.find_last_of("->"s)) == '>';
    }

    std::pair<std::string_view, size_t> ReadDistanceToAnotherStop(const std::string_view &str_v, size_t &pos_begin) {
        size_t distance;
        ReadNumber(str_v, "m"s, pos_begin, distance);
        std::string_view separation = ReadUpToSigns(str_v, " "s, pos_begin);
        std::string_view stop_name = ReadUpToSigns(str_v, ","s, pos_begin);
        return {stop_name, distance};
    }
} // end namespace detail

