#pragma once
#include "dal/IPairAnalyzer.h"
using namespace std;

namespace dal {
class CorrelationAnalyzer final : public IPairAnalyzer {
public:
    double analyze(const ColumnBase& first, const ColumnBase& second) const override;
    string name() const override;
};
}
