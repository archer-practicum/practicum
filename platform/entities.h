#pragma once

struct DayState {

    DayState operator+(const DayState& other) const {        
        return {earned + other.earned, spent + other.spent};
    }

    double ComputeIncome() const {
        return earned - spent;
    }

    double earned = 0.0;
    double spent = 0.0;
};