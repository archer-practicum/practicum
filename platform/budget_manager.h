#pragma once
#include "date.h"

#include <vector>

struct InfoPayDay {
    
};

class BudgetManager {
public:
    inline static const Date START_DATE{2000, 1, 1};
    inline static const Date END_DATE{2100, 1, 1};

    explicit BudgetManager();
    void Earn(Date from, Date to, double count);
    double ComputeIncome(Date from, Date to);
    void PayTax(Date from, Date to, int interest_rate);
    void Spend(Date from, Date to, double count);

private:
    std::vector<double> m_statistics;
};
