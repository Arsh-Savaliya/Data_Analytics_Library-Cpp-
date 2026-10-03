#include "dal/Visualizer.h"
#include "dal/exceptions.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

namespace dal {

HistogramViz::HistogramViz(size_t bins)
    : bins_(bins) {
    if (bins_ == 0U) {
        throw invalid_argument("Histogram requires at least one bin");
    }
}

void HistogramViz::render(const ColumnBase& column, ostream& output) const {
    if (!column.isNumeric()) {
        throw TypeMismatch("Histogram requires a numeric column");
    }

    vector<double> values;
    for (size_t row = 0; row < column.size(); ++row) {
        if (!column.isMissing(row)) {
            values.push_back(column.toDouble(row));
        }
    }

    if (values.empty()) {
        throw EmptyColumn("Histogram requires at least one non-missing value");
    }

    const auto smallest = min_element(values.begin(), values.end());
    const auto largest = max_element(values.begin(), values.end());
    const double lowestValue = *smallest;
    const double highestValue = *largest;
    const double range = highestValue - lowestValue;

    vector<size_t> binCounts(bins_, 0U);
    for (double value : values) {
        size_t bin = 0U;
        if (range != 0.0) {
            const double relativePosition = (value - lowestValue) / range;
            bin = static_cast<size_t>(relativePosition * bins_);
            if (bin >= bins_) {
                bin = bins_ - 1U;
            }
        }
        ++binCounts[bin];
    }

    for (size_t bin = 0; bin < binCounts.size(); ++bin) {
        output << bin << " | ";
        for (size_t mark = 0; mark < binCounts[bin]; ++mark) {
            output << '#';
        }
        output << ' ' << binCounts[bin] << '\n';
    }
}

} // namespace dal
