#include <iostream>
#include <string>
#include <sstream>
#include <deque>
#include <vector>
#include <cmath>
#include <locale>

struct Stop {
    std::string name;
    double latitude;
    double longitude;
};

enum RouteFormat{
    default_format,     // формат по умолчанию
    annular,            // кольцувой маршрут
    sequential_reverse  // маршрут последовательно следует от первой до последней остановки и идет в обратном направлении
};

struct Route {
    std::string name;
    std::deque<Stop> stops;
    RouteFormat format;
};

std::deque<Stop> stops;
std::deque<Route> routes;

size_t find_first_of_space_or_comma(std::string &str, size_t pos) {    
    for (char c = str[pos]; pos < str.size() && c != ' ' && c != ',';) {
        c = str[++pos];
    }
    return pos;
}

void read(std::istream &input) {    
    std::string line;
    std::getline(input, line);
    int count_query =  stoi(line);
    std::vector<std::string> query_route;
    query_route.reserve(count_query);
    size_t pos_begin = 0;
    size_t pos_end = 0;
    // while (std::getline(input, line)) {
    //     pos_begin = line.find_first_not_of(' ');        
    //     if (line[pos_begin] == 'B') {
    //         query_route.push_back(move(line));
    //         continue;
    //     }
    //     pos_begin  = line.find_first_not_of(' ', pos_begin + 4); // stop - content 4 char
    //     pos_end = line.find_first_of(':', pos_begin);
    //     std::string name_stop = line.substr(pos_begin, pos_end - pos_begin);

    //     pos_begin = line.find_first_not_of(' ', pos_end + 1);        
    //     pos_end = find_first_of_space_or_comma(line, pos_begin);
    //     std::string str_latitude = line.substr(pos_begin, pos_end - pos_begin);        
    //     double latitude = stod(str_latitude);

    //     pos_begin = line.find_first_of(',', pos_end) + 1;

    //     pos_begin = line.find_first_not_of(' ', pos_begin);
    //     pos_end = line.find_first_of(' ', pos_begin);
    //     std::string str_longitude = line.substr(pos_begin, pos_end - pos_begin);        
    //     double longitude = stod(str_longitude);

    //     stops.push_back({std::move(name_stop), latitude, longitude});
        
    // }

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

        stops.push_back({std::move(stop_name), stod(latitude), stod(longitude)});

    }

    for (const std::string &line : query_route) {

    }

}

int main() {

    // russian global console key
    setlocale(LC_ALL,"Russian");

    std::istringstream sstring{ 
                                " 10\n"
                                "Stop Tolstopaltsevo: 55.611087, 37.208290\n"
                                "Stop Marushkino:  55.595884 ,  37.209755 \n"
                                "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
                                "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
                                "Stop Rasskazovka: 55.632761, 37.333324\n"
                                "Stop Biryulyovo Zapadnoye: 55.574371, 37.651700\n"
                                "Stop Biryusinka: 55.581065, 37.648390\n"
                                "Stop Universam: 55.587655, 37.645687\n"
                                "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
                                "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n"
                                };
    read(sstring);


                                 
}