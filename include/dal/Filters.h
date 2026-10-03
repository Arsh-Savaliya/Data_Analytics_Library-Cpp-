#pragma once

#include "dal/IFilter.h"

#include <memory>
#include <string>

namespace dal {

// Demonstrates a typed comparison predicate and generic programming.
template <typename T>
class ComparisonFilter final : public IFilter {
public:
    enum class Operator { Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual };
    ComparisonFilter(std::string columnName, Operator op, T value)
        : columnName_(std::move(columnName)), op_(op), value_(std::move(value)) {}
    bool matches(const DataSet& data, std::size_t row) const override;
    std::unique_ptr<IFilter> clone() const override {
        return std::make_unique<ComparisonFilter<T>>(*this);
    }
private:
    std::string columnName_;
    Operator op_;
    T value_;
};

// Demonstrates composite pattern by combining predicates with logical AND.
class AndFilter final : public IFilter {
public:
    AndFilter(std::unique_ptr<IFilter> left, std::unique_ptr<IFilter> right);
    bool matches(const DataSet& data, std::size_t row) const override;
    std::unique_ptr<IFilter> clone() const override;
private:
    std::unique_ptr<IFilter> left_;
    std::unique_ptr<IFilter> right_;
};

// Demonstrates composite pattern by combining predicates with logical OR.
class OrFilter final : public IFilter {
public:
    OrFilter(std::unique_ptr<IFilter> left, std::unique_ptr<IFilter> right);
    bool matches(const DataSet& data, std::size_t row) const override;
    std::unique_ptr<IFilter> clone() const override;
private:
    std::unique_ptr<IFilter> left_;
    std::unique_ptr<IFilter> right_;
};

// Demonstrates composite pattern by negating a contained predicate.
class NotFilter final : public IFilter {
public:
    explicit NotFilter(std::unique_ptr<IFilter> child);
    bool matches(const DataSet& data, std::size_t row) const override;
    std::unique_ptr<IFilter> clone() const override;
private:
    std::unique_ptr<IFilter> child_;
};

} // namespace dal
