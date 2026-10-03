#pragma once
#include "dal/NumericColumnAnalyzer.h"
using namespace std;

namespace dal {
// Mode strategy; when several numeric values tie, returns the smallest tied value.
class ModeAnalyzer final : public NumericColumnAnalyzer {
public:
    double analyze(const ColumnBase& column) const override;
    string name() const override;
};
}
