#include "dal/VarianceAnalyzer.h"

#include <stdexcept>
using namespace std;

namespace dal {

double VarianceAnalyzer::analyze(const ColumnBase& column) const {
    const vector<double> numbers = values(column);
    if (sample_ && numbers.size() < 2U) {
        throw domain_error("Sample variance requires at least two values");
    }

    double total = 0.0;
    for (double number : numbers) {
        total += number;
    }
    const double average = total / static_cast<double>(numbers.size());

    double squaredDifferenceTotal = 0.0;
    for (double number : numbers) {
        const double difference = number - average;
        squaredDifferenceTotal += difference * difference;
    }

    size_t divisor = numbers.size();
    if (sample_) {
        divisor -= 1U;
    }
    return squaredDifferenceTotal / static_cast<double>(divisor);
}

string VarianceAnalyzer::name() const {
    return "variance";
}

} // namespace dal
