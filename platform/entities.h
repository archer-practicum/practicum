#pragma once

struct DayState {

    DayState operator+(const DayState& other) const {        
        return {earned + other.earned, spent};
    }

    DayState operator-(const DayState& other) const {
        return {earned, spent + other.spent};
    }

    DayState& operator*(double interest_rate) {
        earned *= interest_rate;
        return *this;
    }

    double ComputeIncome() const {
        return earned;
    }    

    double earned = 0.0;
    double spent = 0.0;
};