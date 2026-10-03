#include "dal/ModeAnalyzer.h"

#include <map>
using namespace std;

namespace dal {

double ModeAnalyzer::analyze(const ColumnBase& column) const {
    const vector<double> numbers = values(column);
    map<double, size_t> counts;

    for (double number : numbers) {
        ++counts[number];
    }

    // map visits values from smallest to largest. Keeping the first tied value
    // makes the result predictable when several values share the highest count.
    double mostCommonNumber = counts.begin()->first;
    size_t highestCount = 0U;
    for (const auto& item : counts) {
        if (item.second > highestCount) {
            mostCommonNumber = item.first;
            highestCount = item.second;
        }
    }
    return mostCommonNumber;
}

string ModeAnalyzer::name() const {
    return "mode";
}

} // namespace dal
