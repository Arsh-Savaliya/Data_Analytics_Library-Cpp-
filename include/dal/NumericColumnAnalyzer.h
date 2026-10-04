#pragma once

#include "dal/IAnalyzer.h"

#include <vector>
using namespace std;

namespace dal {

// Shared validation and collection for numeric analyzer strategies; it does not retain column data.
class NumericColumnAnalyzer : public IAnalyzer {
protected:
    vector<double> values(const ColumnBase& column) const;
    double quantile(const vector<double>& sortedValues, double percentile) const;
};

} // namespace dal
