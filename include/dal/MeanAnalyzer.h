#pragma once
#include "dal/NumericColumnAnalyzer.h"
using namespace std;

namespace dal {
// Mean strategy selected through IAnalyzer polymorphism.
class MeanAnalyzer final : public NumericColumnAnalyzer {
public:
    double analyze(const ColumnBase& column) const override;
    string name() const override;
};
}
