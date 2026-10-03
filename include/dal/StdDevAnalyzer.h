#pragma once
#include "dal/NumericColumnAnalyzer.h"
using namespace std;

namespace dal {
// Standard-deviation strategy; sample=true uses n-1, otherwise population uses n.
class StdDevAnalyzer final : public NumericColumnAnalyzer {
public:
    explicit StdDevAnalyzer(bool sample = true) : sample_(sample) {}
    double analyze(const ColumnBase& column) const override;
    string name() const override;
private:
    bool sample_;
};
}
