#pragma once

#include "entities.h"
#include "summing_segment_tree.h"

#include <cstdint>
#include <cmath>

struct BulkMoneyAdder {
    DayState delta = {};
};

struct BulkTaxApplier {
    double factor = 1.0;
};

class BulkLinearUpdater {
public:
    BulkLinearUpdater() = default;

    BulkLinearUpdater(const BulkMoneyAdder& add)
        : add_(add) {
    }

    BulkLinearUpdater(const BulkTaxApplier& tax)
        : tax_(tax) {
    }

    void CombineWith(const BulkLinearUpdater& other) {
        tax_.factor *= other.tax_.factor;
        add_.delta.earned = add_.delta.earned * other.tax_.factor + other.add_.delta.earned;
        add_.delta.spent += other.add_.delta.spent;
    }

    DayState Collapse(DayState origin, IndexSegment segment) const {
        //return origin * tax_.factor + add_.delta * static_cast<double>(segment.length());
        return {origin.earned * tax_.factor + add_.delta.earned * static_cast<double>(segment.length())
                , origin.spent + add_.delta.spent * static_cast<double>(segment.length())};
    }

private:
    // apply tax first, then add
    BulkTaxApplier tax_;
    BulkMoneyAdder add_;
};
