#pragma once
#include "dal/IAnalyzer.h"
#include <memory>
using namespace std;
namespace dal {
// Decorator keeps the IAnalyzer interface while adding behavior around another analyzer.
class AnalyzerDecorator : public IAnalyzer {
public:
    explicit AnalyzerDecorator(unique_ptr<IAnalyzer> wrapped);
    string name() const override;
protected:
    unique_ptr<IAnalyzer> wrapped_;
};
class LoggingAnalyzer final : public AnalyzerDecorator {
public:
    explicit LoggingAnalyzer(unique_ptr<IAnalyzer> wrapped);
    double analyze(const ColumnBase& column) const override;
};
class RoundingAnalyzer final : public AnalyzerDecorator {
public:
    RoundingAnalyzer(unique_ptr<IAnalyzer> wrapped, int decimals);
    double analyze(const ColumnBase& column) const override;
private: int decimals_;
};
}
