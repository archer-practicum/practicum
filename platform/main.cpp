#include <iostream>
#include <sstream>
#include <locale>
#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

// int main() {

//     TransportCatalogue catalog;

//     // russian global console key
//     setlocale(LC_ALL,"Russian");

//     std::istringstream sstring{ 
//                                 " 11\n"
//                                 "Stop Tolstopaltsevo: 55.611087, 37.208290\n"
//                                 "Stop Marushkino:  55.595884 ,  37.209755 \n"
//                                 "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
//                                 "Bus 750 adc: Tolstopaltsevo - Marushkino - Rasskazovka\n"
//                                 "Bus 1: Tolstopaltsevo\n"                                
//                                 "Stop Rasskazovka: 55.632761, 37.333324\n"
//                                 "Stop Biryulyovo Zapadnoye: 55.574371, 37.651700\n"
//                                 "Stop Biryusinka: 55.581065, 37.648390\n"
//                                 "Stop Universam: 55.587655, 37.645687\n"
//                                 "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
//                                 "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n"
//                                 "Bus 2 2 : Tolstopaltsevo - Marushkino - Rasskazovka\n"
//                                 };
//     DatabaseFilling(catalog, sstring);
//     std::istringstream query {
//                               "4\n"
//                               "Bus 256\n"
//                               "Bus 750 adc\n"
//                               "Bus 1\n"
//                               "Bus 2 2\n"
//                               };
//     DatabaseQueries(catalog, query);
// }

int main() {

    TransportCatalogue catalog;

    // russian global console key
    setlocale(LC_ALL,"Russian");

    DatabaseFilling(catalog, std::cin);
    DatabaseQueries(catalog, std::cin);
}