#pragma once
#include "dal/NumericColumnAnalyzer.h"
using namespace std;

namespace dal {
// Median strategy selected through IAnalyzer polymorphism.
class MedianAnalyzer final : public NumericColumnAnalyzer {
public:
    double analyze(const ColumnBase& column) const override;
    string name() const override;
};
}
