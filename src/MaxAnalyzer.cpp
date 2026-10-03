#include "dal/MaxAnalyzer.h"

#include <algorithm>
using namespace std;

namespace dal {

double MaxAnalyzer::analyze(const ColumnBase& column) const {
    const vector<double> numbers = values(column);
    return *max_element(numbers.begin(), numbers.end());
}

string MaxAnalyzer::name() const {
    return "max";
}

} // namespace dal
