#pragma once

#include "dal/IFilter.h"
#include "dal/DataSet.h"
#include "dal/exceptions.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
using namespace std;

namespace dal {

// Demonstrates a typed comparison predicate and generic programming.
template <typename T>
class ComparisonFilter final : public IFilter {
public:
    enum class Operator {
        EQ, NE, LT, LE, GT, GE,
        Equal = EQ, NotEqual = NE, Less = LT,
        LessEqual = LE, Greater = GT, GreaterEqual = GE
    };

    ComparisonFilter(string columnName, Operator op, T value)
        : columnName_(move(columnName)), op_(op), value_(move(value)) {}

    bool matches(const DataSet& data, size_t row) const override {
        const auto& column = data.template getColumnAs<T>(columnName_);
        const auto& cell = column.at(row);
        if (!cell) {
            return false;
        }

        switch (op_) {
        case Operator::EQ:
            return *cell == value_;
        case Operator::NE:
            return *cell != value_;
        case Operator::LT:
            return *cell < value_;
        case Operator::LE:
            return *cell <= value_;
        case Operator::GT:
            return *cell > value_;
        case Operator::GE:
            return *cell >= value_;
        }
        return false;
    }

    unique_ptr<IFilter> clone() const override {
        return make_unique<ComparisonFilter<T>>(*this);
    }
private:
    string columnName_;
    Operator op_;
    T value_;
};

// Typed string predicate, using the same type-safe DataSet lookup as comparisons.
class StringContainsFilter final : public IFilter {
public:
    StringContainsFilter(string columnName, string text)
        : columnName_(move(columnName)), text_(move(text)) {}

    bool matches(const DataSet& data, size_t row) const override {
        const auto& value = data.getColumnAs<string>(columnName_).at(row);
        return value && value->find(text_) != string::npos;
    }

    unique_ptr<IFilter> clone() const override {
        return make_unique<StringContainsFilter>(*this);
    }

private: string columnName_; string text_;
};

template <typename T>
class RangeFilter final : public IFilter {
public:
    RangeFilter(string columnName, T low, T high)
        : columnName_(move(columnName)), low_(move(low)), high_(move(high)) {}

    bool matches(const DataSet& data, size_t row) const override {
        const auto& value = data.template getColumnAs<T>(columnName_).at(row);
        return value && *value >= low_ && *value <= high_;
    }

    unique_ptr<IFilter> clone() const override {
        return make_unique<RangeFilter<T>>(*this);
    }

private:
    string columnName_;
    T low_;
    T high_;
};

class IsMissingFilter final : public IFilter {
public:
    explicit IsMissingFilter(string columnName) : columnName_(move(columnName)) {}

    bool matches(const DataSet& data, size_t row) const override {
        return data.getColumn(columnName_).isMissing(row);
    }

    unique_ptr<IFilter> clone() const override {
        return make_unique<IsMissingFilter>(*this);
    }

private:
    string columnName_;
};

// Composite handle supports fluent filter expressions while remaining an IFilter.
class FilterHandle final : public IFilter {
public:
    explicit FilterHandle(unique_ptr<IFilter> filter)
        : filter_(move(filter)) {
        if (!filter_) {
            throw invalid_argument("FilterHandle cannot be empty");
        }
    }

    bool matches(const DataSet& data, size_t row) const override {
        return filter_->matches(data, row);
    }

    unique_ptr<IFilter> clone() const override {
        return make_unique<FilterHandle>(filter_->clone());
    }

    unique_ptr<IFilter> release() {
        return move(filter_);
    }

private:
    unique_ptr<IFilter> filter_;
};

// These builder templates keep the selected value type attached to the filter.
template <typename T>
FilterHandle eq(string name, T value) {
    return FilterHandle(make_unique<ComparisonFilter<T>>(move(name), ComparisonFilter<T>::Operator::EQ, move(value)));
}
template <typename T>
FilterHandle ne(string name, T value) {
    return FilterHandle(make_unique<ComparisonFilter<T>>(move(name), ComparisonFilter<T>::Operator::NE, move(value)));
}
template <typename T>
FilterHandle gt(string name, T value) {
    return FilterHandle(make_unique<ComparisonFilter<T>>(move(name), ComparisonFilter<T>::Operator::GT, move(value)));
}
template <typename T>
FilterHandle ge(string name, T value) {
    return FilterHandle(make_unique<ComparisonFilter<T>>(move(name), ComparisonFilter<T>::Operator::GE, move(value)));
}
template <typename T>
FilterHandle lt(string name, T value) {
    return FilterHandle(make_unique<ComparisonFilter<T>>(move(name), ComparisonFilter<T>::Operator::LT, move(value)));
}
template <typename T>
FilterHandle le(string name, T value) {
    return FilterHandle(make_unique<ComparisonFilter<T>>(move(name), ComparisonFilter<T>::Operator::LE, move(value)));
}
inline FilterHandle contains(string name, string text) {
    return FilterHandle(make_unique<StringContainsFilter>(move(name), move(text)));
}

template <typename T>
FilterHandle between(string name, T low, T high) {
    return FilterHandle(make_unique<RangeFilter<T>>(move(name), move(low), move(high)));
}

inline FilterHandle isMissing(string name) {
    return FilterHandle(make_unique<IsMissingFilter>(move(name)));
}

// Demonstrates composite pattern by combining predicates with logical AND.
class AndFilter final : public IFilter {
public:
    AndFilter(unique_ptr<IFilter> left, unique_ptr<IFilter> right);
    bool matches(const DataSet& data, size_t row) const override;
    unique_ptr<IFilter> clone() const override;
private:
    unique_ptr<IFilter> left_;
    unique_ptr<IFilter> right_;
};

// Demonstrates composite pattern by combining predicates with logical OR.
class OrFilter final : public IFilter {
public:
    OrFilter(unique_ptr<IFilter> left, unique_ptr<IFilter> right);
    bool matches(const DataSet& data, size_t row) const override;
    unique_ptr<IFilter> clone() const override;
private:
    unique_ptr<IFilter> left_;
    unique_ptr<IFilter> right_;
};

// Demonstrates composite pattern by negating a contained predicate.
class NotFilter final : public IFilter {
public:
    explicit NotFilter(unique_ptr<IFilter> child);
    bool matches(const DataSet& data, size_t row) const override;
    unique_ptr<IFilter> clone() const override;
private:
    unique_ptr<IFilter> child_;
};

inline FilterHandle operator&&(FilterHandle left, FilterHandle right) {
    return FilterHandle(make_unique<AndFilter>(left.release(), right.release()));
}
inline FilterHandle operator||(FilterHandle left, FilterHandle right) {
    return FilterHandle(make_unique<OrFilter>(left.release(), right.release()));
}
inline FilterHandle operator!(FilterHandle child) {
    return FilterHandle(make_unique<NotFilter>(child.release()));
}

} // namespace dal
