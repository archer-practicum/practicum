#include "input_reader.h"
#include <unordered_map>

void DatabaseFilling(TransportCatalogue &catalogue, std::istream &input) {

    const auto &queries = ReadQueries(input);
    std::unordered_map<const Stop *, std::string_view> stop_and_disnance_other_stops;

    for (const std::string &query : queries.stops) {        
        catalogue.AddStop(std::move(ReadStop(query)));
    }

    for (const std::string &query : queries.buses) {
        catalogue.AddBus(std::move(ReadBus(query, catalogue)));
    }
}

TypesQueries ReadQueries(std::istream &input) {
    int count_queries;
    TypesQueries queries;
    input >> count_queries;
    input.ignore(1);

    for (int i = 0; i < count_queries; ++i) {
        std::string line;
        std::getline(input, line);
        if (line[0] == 'B') {
            queries.buses.push_back(std::move(line));
        } else {
            queries.stops.push_back(std::move(line));
        }        
    }

    return queries;
}

Bus ReadBus(const std::string &query, TransportCatalogue &catalogue) {
    size_t pos_begin = query.find_first_not_of(' ', 3); // bus - content 3 char
    size_t pos_end = query.find_first_of(':', pos_begin);
    std::string bus_name = query.substr(pos_begin, pos_end - pos_begin);

    std::deque<const Stop *> stops;
    pos_begin = pos_end + 1;
    
    while (1) {
        pos_begin = query.find_first_not_of(' ', pos_begin); // begin stop name
        if(pos_begin == query.npos) break;
        size_t pos_delimiter = query.find_first_of(std::string("->"), pos_begin); // find delimiter
        pos_end = query.find_last_not_of(' ', pos_delimiter - 1) + 1;    // +1 - результат работы ф. указывает на последнюю букву имени остановки
        std::string stop_name = query.substr(pos_begin, pos_end - pos_begin); 
        stops.push_back(catalogue.GetStop(stop_name));
        if (pos_delimiter == query.npos) break;
        pos_begin = pos_delimiter + 1;
    }

    bool is_circule = query.at(query.find_last_of(std::string("->"))) == '>';
    return {std::move(bus_name), std::move(stops), is_circule};
}

Stop ReadStop(const std::string &query) {
    size_t pos_begin = query.find_first_not_of(' ', 4); // stop - content 4 char
    size_t pos_end = query.find_first_of(':', pos_begin);
    std::string stop_name = query.substr(pos_begin, pos_end - pos_begin);

    pos_begin = pos_end + 1;
    pos_end = query.find_first_of(',', pos_begin);
    std::string latitude = query.substr(pos_begin, pos_end - pos_begin);

    pos_begin = pos_end + 1;
    pos_end = query.find_first_of(',', pos_begin);
    std::string longitude = query.substr(pos_begin, pos_end - pos_begin);

    return {std::move(stop_name), stod(latitude), stod(longitude)};
}

std::string_view SkipSpaceReadUpToSign(const std::string &str, char sign, size_t &pos_begin, size_t pos_end) {
    pos_begin = str.find_first_not_of(' ');
    pos_end = str.find_first_of(':', pos_begin);
    std::string stop_name = str.substr(pos_begin, pos_end - pos_begin);
    return {&str[pos_begin], pos_end - pos_begin};
}