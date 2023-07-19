#pragma once

#include "transport_catalogue.h"
#include <iostream>
#include <deque>
#include <string>
#include <string_view>
#include <charconv>

void DatabaseFilling(TransportCatalogue &catalogue, std::istream &input);

namespace detail {

    std::deque<std::string> ReadQueries(std::istream &input);

    Bus ReadBus(const TransportCatalogue &catalogue, const std::string_view &str_v);

    Stop ReadStop(const std::string_view &str_v, size_t &pos_begin);


    // skips spaces at the beginning and end
    std::string_view ReadUpToSigns(const std::string_view &str_v, const std::string &signs, size_t &pos_begin);

    std::deque<const Stop *> ReadBusRoute(const TransportCatalogue &catalogue, const std::string_view &str_v, size_t &pos_begin);

    bool IsCircularRoute(const std::string_view &str_v);

    std::pair<std::string_view, size_t> ReadDistanceToAnotherStop(const std::string_view &str_v, size_t &pos_begin);

    template<typename Type>
    void ReadNumber(const std::string_view& str_v, const std::string &signs, size_t &pos_begin, Type &number) {
        std::string_view number_sv = ReadUpToSigns(str_v, signs, pos_begin);    
        std::from_chars(number_sv.data(), number_sv.data() + number_sv.size(), number);    
    }
} // end namespace dateil

