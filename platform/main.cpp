#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

// не меняйте файлы xml.h и xml.cpp
#include "xml.h"

using namespace std;

struct Spending {
    string category;
    int amount;
};

int CalculateTotalSpendings(const vector<Spending>& spendings) {
    return accumulate(spendings.begin(), spendings.end(), 0,
                      [](int current, const Spending& spending) {
                          return current + spending.amount;
                      });
}

string FindMostExpensiveCategory(const vector<Spending>& spendings) {
    assert(!spendings.empty());
    auto compare_by_amount = [](const Spending& lhs, const Spending& rhs) {
        return lhs.amount < rhs.amount;
    };
    return max_element(begin(spendings), end(spendings), compare_by_amount)->category;
}

vector<Spending> LoadFromXml(istream& input) {
    vector<Spending> res;
    // Document doc = Load(input);
    // for (const auto &node : doc.GetRoot().Children()) {
    //     string category = node.AttributeValue<string>("category"s);
    //     int amount = node.AttributeValue<int>("amount"s);
    //     res.push_back({move(category), amount});
    // }
    return res;
}

int main() {
    const vector<Spending> spendings = LoadFromXml(cin);
    cout << "Total "sv << CalculateTotalSpendings(spendings) << '\n';
    cout << "Most expensive is "sv << FindMostExpensiveCategory(spendings) << '\n';
}