

// #include "budget_manager.h"

// BudgetManager::BudgetManager() : m_statistics(Date::ComputeDistance(START_DATE, END_DATE) + 1) {} // +1 - нет нулевого дня

// void BudgetManager::Earn(Date from, Date to, double income) {
//     int begin = Date::ComputeDistance(START_DATE, from);
//     int end = Date::ComputeDistance(START_DATE, to);
//     income /= (end - begin + 1);

//     for (; begin <= end; ++begin) {
//         m_statistics[begin].income += income;
//     }
// }

// double BudgetManager::ComputeIncome(Date from, Date to) {
//     double res = 0.0;
//     int begin = Date::ComputeDistance(START_DATE, from);
//     int end = Date::ComputeDistance(START_DATE, to);    

//     for (; begin <= end; ++begin) {
//         res += m_statistics[begin].income - m_statistics[begin].spending;
//     }

//     return res;
// }

// void BudgetManager::PayTax(Date from, Date to, int interest_rate) {
//     int begin = Date::ComputeDistance(START_DATE, from);
//     int end = Date::ComputeDistance(START_DATE, to);

//     double proc = 1.0 - static_cast<double>(interest_rate) / 100;

//     for (; begin <= end; ++begin) {
//         m_statistics[begin].income *= proc;
//     } 
// }

// void BudgetManager::Spend(Date from, Date to, double spending) {
//     int begin = Date::ComputeDistance(START_DATE, from);
//     int end = Date::ComputeDistance(START_DATE, to);

//     spending /= (end - begin + 1);

//     for (; begin <= end; ++begin) {
//         m_statistics[begin].spending += spending;
//     } 
// }