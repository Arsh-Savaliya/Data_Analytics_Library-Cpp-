#pragma once
#include "dal/ColumnBase.h"

#include <string>

namespace dal {

// Demonstrates a strategy interface for interchangeable analytics algorithms.
class IAnalyzer {
public:
    virtual ~IAnalyzer() = default;
    virtual double analyze(const ColumnBase& column) const = 0;
    virtual std::string name() const = 0;
};

// Demonstrates strategy polymorphism with a mean calculation.
class MeanAnalyzer final : public IAnalyzer {
public:
    double analyze(const ColumnBase& column) const override;
    std::string name() const override;
};

// Demonstrates strategy polymorphism with a median calculation.
class MedianAnalyzer final : public IAnalyzer {
public:
    double analyze(const ColumnBase& column) const override;
    std::string name() const override;
};

} // namespace dal
