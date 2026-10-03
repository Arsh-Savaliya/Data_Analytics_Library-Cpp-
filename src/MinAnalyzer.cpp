#include "dal/MinAnalyzer.h"

#include <algorithm>
using namespace std;

namespace dal {

double MinAnalyzer::analyze(const ColumnBase& column) const {
    const vector<double> numbers = values(column);
    return *min_element(numbers.begin(), numbers.end());
}

string MinAnalyzer::name() const {
    return "min";
}

} // namespace dal
