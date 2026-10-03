#pragma once
#include "dal/NumericColumnAnalyzer.h"
using namespace std;

namespace dal {
class SumAnalyzer final : public NumericColumnAnalyzer {
public:
    double analyze(const ColumnBase& column) const override;
    string name() const override;
};
}
