#include "dal/SumAnalyzer.h"
using namespace std;

namespace dal {

double SumAnalyzer::analyze(const ColumnBase& column) const {
    const vector<double> numbers = values(column);

    double total = 0.0;
    for (double number : numbers) {
        total += number;
    }
    return total;
}

string SumAnalyzer::name() const {
    return "sum";
}

} // namespace dal
