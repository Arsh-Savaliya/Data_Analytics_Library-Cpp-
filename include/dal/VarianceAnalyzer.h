#pragma once
#include "dal/NumericColumnAnalyzer.h"
using namespace std;

namespace dal {
// Variance strategy with the same sample/population choice as standard deviation.
class VarianceAnalyzer final : public NumericColumnAnalyzer {
public:
    explicit VarianceAnalyzer(bool sample = true) : sample_(sample) {}
    double analyze(const ColumnBase& column) const override;
    string name() const override;
private:
    bool sample_;
};
}
