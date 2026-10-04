#include "dal/NumericColumnAnalyzer.h"
#include "dal/exceptions.h"

#include <cmath>
using namespace std;

namespace dal {

vector<double> NumericColumnAnalyzer::values(const ColumnBase& column) const {
    if (!column.isNumeric()) {
        throw TypeMismatch("Analyzer requires a numeric column");
    }

    vector<double> result;
    for (size_t row = 0; row < column.size(); ++row) {
        if (!column.isMissing(row)) {
            result.push_back(column.toDouble(row));
        }
    }

    if (result.empty()) {
        throw EmptyColumn("Analyzer requires at least one non-missing value");
    }
    return result;
}

double NumericColumnAnalyzer::quantile(
    const vector<double>& sortedValues,
    double percentile) const {
    const double lastIndex = static_cast<double>(sortedValues.size() - 1U);
    const double position = percentile / 100.0 * lastIndex;
    const size_t lowerIndex = static_cast<size_t>(floor(position));
    const size_t upperIndex = static_cast<size_t>(ceil(position));
    const double distance = position - static_cast<double>(lowerIndex);

    const double lowerValue = sortedValues[lowerIndex];
    const double upperValue = sortedValues[upperIndex];
    return lowerValue + (upperValue - lowerValue) * distance;
}

} // namespace dal
