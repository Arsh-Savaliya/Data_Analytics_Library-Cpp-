#include "dal/Visualizer.h"
#include "dal/exceptions.h"

#include <algorithm>
#include <cmath>
#include <vector>
using namespace std;

namespace dal {
double BoxSummaryViz::quantile(
    const vector<double>& sortedValues,
    double percentage) const {
    const double lastIndex = static_cast<double>(sortedValues.size() - 1U);
    const double position = percentage * lastIndex;
    const size_t lowerIndex = static_cast<size_t>(floor(position));
    const size_t upperIndex = static_cast<size_t>(ceil(position));
    const double distance = position - static_cast<double>(lowerIndex);

    const double lowerValue = sortedValues[lowerIndex];
    const double upperValue = sortedValues[upperIndex];
    return lowerValue + (upperValue - lowerValue) * distance;
}

void BoxSummaryViz::render(const ColumnBase& column, ostream& output) const {
    if (!column.isNumeric()) {
        throw TypeMismatch("Box summary requires a numeric column");
    }

    vector<double> values;
    for (size_t row = 0; row < column.size(); ++row) {
        if (!column.isMissing(row)) {
            values.push_back(column.toDouble(row));
        }
    }

    if (values.empty()) {
        throw EmptyColumn("Box summary requires at least one non-missing value");
    }

    sort(values.begin(), values.end());
    const double firstQuartile = quantile(values, 0.25);
    const double median = quantile(values, 0.50);
    const double thirdQuartile = quantile(values, 0.75);

    output << values.front() << " |--[ " << firstQuartile << " | " << median << " | "
           << thirdQuartile << " ]--| " << values.back() << '\n';
}

} // namespace dal
