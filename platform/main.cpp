#include "budget_manager.h"

#include "parser.h"

#include <iostream>
#include <string_view>
#include <cassert>
#include <cmath>
#include <sstream>

using namespace std;

// void ParseAndProcessQuery(BudgetManager& manager, const std::string& line) {    
//     std::stringstream input(line);

//     std::string type_query;
//     input >> type_query;
//     if (type_query == "Earn") {
//         std::string date_from, date_to;
//         double count;
//         input >> date_from >> date_to >> count;
//         manager.Earn(Date::FromString(date_from), Date::FromString(date_to), count);
//     } else if (type_query == "ComputeIncome") {
//         std::string date_from, date_to;        
//         input >> date_from >> date_to;
//         std::cout << manager.ComputeIncome(Date::FromString(date_from), Date::FromString(date_to)) << std::endl;
//     } else if (type_query == "PayTax") {
//         std::string date_from, date_to;
//         int proc;
//         input >> date_from >> date_to >> proc;
//         manager.PayTax(Date::FromString(date_from), Date::FromString(date_to), proc);
//     } else if (type_query == "Spend") {
//         std::string date_from, date_to;
//         double spending;
//         input >> date_from >> date_to >> spending;
//         manager.Spend(Date::FromString(date_from), Date::FromString(date_to), spending);
//     }
// }

int ReadNumberOnLine(istream& input) {
    std::string line;
    std::getline(input, line);
    return std::stoi(line);
}

bool CompareDouble(double d1, double d2) {
    static const double exponent = 10E-6;
    return std::abs(d1 - d2) < exponent;
}

// void TestBudgetManager() {
//     BudgetManager manager;

//     manager.Earn({2000, 1, 2}, {2000, 1, 6}, 20);
//     assert(CompareDouble(manager.ComputeIncome({2000, 1, 1}, {2001, 1, 1}), 20));

//     manager.PayTax({2000, 1, 2}, {2000, 1, 3}, 13);
//     assert(CompareDouble(manager.ComputeIncome({2000, 1, 1}, {2001, 1, 1}), 18.96));

//     manager.Spend({2000, 12, 30}, {2001, 1, 2}, 14);    
//     assert(CompareDouble(manager.ComputeIncome({2000, 1, 1}, {2001, 1, 1}), 8.46));

//     manager.PayTax({2000, 12, 30}, {2000, 12, 30}, 13);
//     assert(CompareDouble(manager.ComputeIncome({2000, 1, 1}, {2001, 1, 1}), 8.46));

// }

void ParseAndProcessQuery(BudgetManager& manager, string_view line) {
    auto query = ParseQuery(line);

    if (!query) {
        return;
    }

    query->ProcessAndPrint(manager, cout);
}

int main() {
    //TestBudgetManager();

    
    BudgetManager manager;

    const int query_count = ReadNumberOnLine(cin);

    for (int i = 0; i < query_count; ++i) {
        std::string line;
        std::getline(cin, line);
        ParseAndProcessQuery(manager, line);
    }
}
