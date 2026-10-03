#include "dal/StdDevAnalyzer.h"

#include <cmath>
#include <stdexcept>
using namespace std;

namespace dal {

double StdDevAnalyzer::analyze(const ColumnBase& column) const {
    const vector<double> numbers = values(column);
    if (sample_ && numbers.size() < 2U) {
        throw domain_error("Sample standard deviation requires at least two values");
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
    const double variance = squaredDifferenceTotal / static_cast<double>(divisor);
    return sqrt(variance);
}

string StdDevAnalyzer::name() const {
    return "stddev";
}

} // namespace dal
