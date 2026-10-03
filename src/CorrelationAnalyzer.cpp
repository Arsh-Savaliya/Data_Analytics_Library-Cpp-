#include "dal/CorrelationAnalyzer.h"
#include "dal/exceptions.h"

#include <cmath>
#include <stdexcept>
#include <vector>
using namespace std;

namespace dal {

double CorrelationAnalyzer::analyze(
    const ColumnBase& first,
    const ColumnBase& second) const {
    if (!first.isNumeric() || !second.isNumeric()) {
        throw TypeMismatch("Correlation requires numeric columns");
    }
    if (first.size() != second.size()) {
        throw invalid_argument("Paired columns must have equal lengths");
    }

    vector<double> firstValues;
    vector<double> secondValues;

    // A pair is usable only when neither cell is missing.
    for (size_t row = 0; row < first.size(); ++row) {
        if (!first.isMissing(row) && !second.isMissing(row)) {
            firstValues.push_back(first.toDouble(row));
            secondValues.push_back(second.toDouble(row));
        }
    }

    if (firstValues.empty()) {
        throw EmptyColumn("Correlation has no complete pairs");
    }
    if (firstValues.size() < 2U) {
        throw domain_error("Correlation requires at least two complete pairs");
    }

    double firstMean = 0.0;
    double secondMean = 0.0;
    for (size_t row = 0; row < firstValues.size(); ++row) {
        firstMean += firstValues[row];
        secondMean += secondValues[row];
    }
    firstMean /= static_cast<double>(firstValues.size());
    secondMean /= static_cast<double>(secondValues.size());

    double together = 0.0;
    double firstSquaredDifferences = 0.0;
    double secondSquaredDifferences = 0.0;
    for (size_t row = 0; row < firstValues.size(); ++row) {
        const double firstDifference = firstValues[row] - firstMean;
        const double secondDifference = secondValues[row] - secondMean;

        together += firstDifference * secondDifference;
        firstSquaredDifferences += firstDifference * firstDifference;
        secondSquaredDifferences += secondDifference * secondDifference;
    }

    if (firstSquaredDifferences == 0.0 || secondSquaredDifferences == 0.0) {
        throw domain_error("Correlation is undefined for a constant column");
    }

    return together / sqrt(firstSquaredDifferences * secondSquaredDifferences);
}

string CorrelationAnalyzer::name() const {
    return "correlation";
}

} // namespace dal
