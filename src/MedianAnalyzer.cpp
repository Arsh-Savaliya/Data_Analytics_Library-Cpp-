#include "dal/MedianAnalyzer.h"

#include <algorithm>
using namespace std;

namespace dal {

double MedianAnalyzer::analyze(const ColumnBase& column) const {
    vector<double> numbers = values(column);
    sort(numbers.begin(), numbers.end());
    return quantile(numbers, 50.0);
}

string MedianAnalyzer::name() const {
    return "median";
}

} // namespace dal
