#pragma once
#include "dal/NumericColumnAnalyzer.h"
using namespace std;

namespace dal {
class PercentileAnalyzer final : public NumericColumnAnalyzer {
public:
    explicit PercentileAnalyzer(double percentile);
    double analyze(const ColumnBase& column) const override;
    string name() const override;
private: double percentile_;
};
}
