#include "dal/CovarianceAnalyzer.h"
#include "dal/exceptions.h"

#include <stdexcept>
#include <vector>
using namespace std;

namespace dal {

double CovarianceAnalyzer::analyze(
    const ColumnBase& first,
    const ColumnBase& second) const {
    if (!first.isNumeric() || !second.isNumeric()) {
        throw TypeMismatch("Covariance requires numeric columns");
    }
    if (first.size() != second.size()) {
        throw invalid_argument("Paired columns must have equal lengths");
    }

    vector<double> firstValues;
    vector<double> secondValues;
    for (size_t row = 0; row < first.size(); ++row) {
        if (!first.isMissing(row) && !second.isMissing(row)) {
            firstValues.push_back(first.toDouble(row));
            secondValues.push_back(second.toDouble(row));
        }
    }

    if (firstValues.empty()) {
        throw EmptyColumn("Covariance has no complete pairs");
    }
    if (sample_ && firstValues.size() < 2U) {
        throw domain_error("Sample covariance requires at least two complete pairs");
    }

    double firstMean = 0.0;
    double secondMean = 0.0;
    for (size_t row = 0; row < firstValues.size(); ++row) {
        firstMean += firstValues[row];
        secondMean += secondValues[row];
    }
    firstMean /= static_cast<double>(firstValues.size());
    secondMean /= static_cast<double>(secondValues.size());

    double productSum = 0.0;
    for (size_t row = 0; row < firstValues.size(); ++row) {
        const double firstDifference = firstValues[row] - firstMean;
        const double secondDifference = secondValues[row] - secondMean;
        productSum += firstDifference * secondDifference;
    }

    size_t divisor = firstValues.size();
    if (sample_) {
        divisor -= 1U;
    }
    return productSum / static_cast<double>(divisor);
}

string CovarianceAnalyzer::name() const {
    return "covariance";
}

} // namespace dal
