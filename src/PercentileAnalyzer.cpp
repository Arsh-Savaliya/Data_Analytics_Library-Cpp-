#include "dal/PercentileAnalyzer.h"
#include <algorithm>
#include <stdexcept>
using namespace std;

namespace dal {
PercentileAnalyzer::PercentileAnalyzer(double percentile) : percentile_(percentile) {
    if (percentile < 0.0 || percentile > 100.0) {
        throw invalid_argument("Percentile must be between 0 and 100");
    }
}

double PercentileAnalyzer::analyze(const ColumnBase& column) const {
    auto data = values(column);
    sort(data.begin(), data.end());
    return quantile(data, percentile_);
}

string PercentileAnalyzer::name() const {
    return "percentile";
}
}
