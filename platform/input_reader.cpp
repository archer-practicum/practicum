#include "input_reader.h"
#include <deque>
#include <string>
#include <limits>

void DatabaseFilling(TransportCatalogue &catalog, std::istream &input) {
    
    int count_query;        
    for(;;) {
        input >> count_query;
 
        if(input.eof() || input.bad())
            return;
        else if(input.fail()) {
            input.clear(); // unset failbit
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // skip bad input
        } else {
            break;
        }
    }
    input.ignore(1);
    std::deque<std::string> all_query;
    
    for (int i = 0; i < count_query; ++i) {
        std::string line;
        std::getline(input, line);
        all_query.push_back(std::move(line));
    }
    
    std::deque<std::string> query_bus;
    size_t pos_begin = 0;
    size_t pos_end = 0;

    for (const std::string &query : all_query) {
        pos_begin = query.find_first_not_of(' ');

        if (query[pos_begin] == 'B') {
            query_bus.push_back(move(query));
            continue;
        }

        pos_begin = query.find_first_not_of(' ', pos_begin + 4); // stop - content 4 char
        pos_end = query.find_first_of(':', pos_begin);
        std::string stop_name = query.substr(pos_begin, pos_end - pos_begin);

        pos_begin = pos_end + 1;
        pos_end = query.find_first_of(',', pos_begin);
        std::string latitude = query.substr(pos_begin, pos_end - pos_begin);

        pos_begin = pos_end + 1;        
        std::string longitude = query.substr(pos_begin);

        catalog.AddStop({std::move(stop_name), stod(latitude), stod(longitude)});        
    }

    for (const std::string &query : query_bus) {
        pos_begin = query.find_first_not_of(' ');
        pos_begin = query.find_first_not_of(' ', pos_begin + 3); // bus - content 3 char
        pos_end = query.find_first_of(':', pos_begin);
        std::string bus_name = query.substr(pos_begin, pos_end - pos_begin);

        std::deque<const Stop*> stops;
        pos_begin = pos_end + 1;
        size_t pos_delimiter = query.find_first_of(std::string("->"), pos_begin);
        bool is_circule = query.at(pos_delimiter) == '>';
        
        while (1) {
            pos_begin = query.find_first_not_of(' ', pos_begin); // begin stop name
            if(pos_begin == query.npos) break;
            pos_delimiter = query.find_first_of(std::string("->"), pos_begin); // find delimiter
            pos_end = query.find_last_not_of(' ', pos_delimiter - 1) + 1;    // +1 - результат работы ф. указывает на последнюю букву имени остановки
            std::string stop_name = query.substr(pos_begin, pos_end - pos_begin); 
            stops.push_back(catalog.GetStop(stop_name));
            if (pos_delimiter == query.npos) break;
            pos_begin = pos_delimiter + 1;
        }
        
        catalog.AddBus({std::move(bus_name), std::move(stops), is_circule});
    }
}