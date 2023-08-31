#include "parser.h"

using namespace std;

namespace queries {
class ComputeIncome : public ComputeQuery {
public:
    using ComputeQuery::ComputeQuery;
    ReadResult Process(const BudgetManager& budget) const override {
        return {budget.ComputeSum(GetFrom(), GetTo())};
    }

    class Factory : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            return std::make_unique<ComputeIncome>(Date::FromString(parts[0]), Date::FromString(parts[1]));
        }
    };
};

class Alter : public ModifyQuery {
public:
    Alter(Date from, Date to, double amount)
        : ModifyQuery(from, to)
        , amount_(amount) {
    }

    void Process(BudgetManager& budget) const override {
        if (amount_ > 0) {
            double day_income = amount_ / (Date::ComputeDistance(GetFrom(), GetTo()) + 1);

            budget.AddBulkOperation(GetFrom(), GetTo(), BulkMoneyAdder{day_income, 0.0});
        } else {
            double day_outcome = -amount_ / (Date::ComputeDistance(GetFrom(), GetTo()) + 1);

            budget.AddBulkOperation(GetFrom(), GetTo(), BulkMoneyAdder{0.0, day_outcome});
        }
        
    }

    class FactoryEarn : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            double payload = std::stod(std::string(parts[2]));
            return std::make_unique<Alter>(Date::FromString(parts[0]), Date::FromString(parts[1]), payload);
        }
    };

    class FactorySpend : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            double payload = std::stod(std::string(parts[2]));
            return std::make_unique<Alter>(Date::FromString(parts[0]), Date::FromString(parts[1]), -payload);
        }
    };

private:
    double amount_;
};

class PayTax : public ModifyQuery {
public:
    using ModifyQuery::ModifyQuery;

    PayTax(Date from, Date to) = delete;

    PayTax(Date from, Date to, int interest_rate) : ModifyQuery(from, to), m_interest_rate(interest_rate) {}

    void Process(BudgetManager& budget) const override {        
        budget.AddBulkOperation(GetFrom(), GetTo(), BulkTaxApplier{1.0 - static_cast<double>(m_interest_rate) / 100.0});
    }

    class Factory : public QueryFactory {
    public:
        std::unique_ptr<Query> Construct(std::string_view config) const override {
            auto parts = Split(config, ' ');
            return std::make_unique<PayTax>(Date::FromString(parts[0]), Date::FromString(parts[1]), stoi(std::string(parts[2])));
        }
    };

private:
    int m_interest_rate = 0;
};

}  // namespace queries

const QueryFactory& QueryFactory::GetFactory(std::string_view id) {
    static queries::ComputeIncome::Factory compute_income;
    static queries::Alter::FactoryEarn earn;
    static queries::Alter::FactorySpend spend;
    static queries::PayTax::Factory pay_tax;
    static std::unordered_map<std::string_view, const QueryFactory&> factories
        = {{"ComputeIncome"sv, compute_income}, {"Earn"sv, earn}, {"PayTax"sv, pay_tax}, {"Spend"sv, spend}};

    return factories.at(id);
}
