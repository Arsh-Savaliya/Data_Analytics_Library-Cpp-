#pragma once
#include "dal/IPairAnalyzer.h"
using namespace std;

namespace dal {
class CovarianceAnalyzer final : public IPairAnalyzer {
public:
    explicit CovarianceAnalyzer(bool sample = true) : sample_(sample) {}
    double analyze(const ColumnBase& first, const ColumnBase& second) const override;
    string name() const override;
private: bool sample_;
};
}
