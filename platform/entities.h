#pragma once

struct DailyAccouting {

    DailyAccouting& operator+(const DailyAccouting& other) {
        income += other.income;
        spending += other.spending;
        return *this;
    }

    double income = 0.0;
    double spending = 0.0;
};