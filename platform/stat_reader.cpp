#include "stat_reader.h"
#include <string>
#include <deque>
#include <limits>

void DatabaseQueries(const TransportCatalogue &catalog, std::istream &input) {
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

    size_t pos_begin = 0;    

    for (const std::string &query : all_query) {
        pos_begin = query.find_first_not_of(' '); // пропускаем пробелы до начала первого слова
        pos_begin = query.find_first_not_of(' ', pos_begin + 3); // bus - content 3 char        
        std::string bus_name = query.substr(pos_begin);
        std::cout << catalog.GetInfoBus(bus_name) << std::endl;
    }
}