#include "dal/Visualizer.h"
#include "dal/exceptions.h"

#include <map>
#include <string>
using namespace std;

namespace dal {

void BarChartViz::render(const ColumnBase& column, ostream& output) const {
    if (column.isNumeric()) {
        throw TypeMismatch("BarChartViz requires a categorical column");
    }

    map<string, size_t> categoryCounts;
    for (size_t row = 0; row < column.size(); ++row) {
        if (!column.isMissing(row)) {
            const string category = column.valueAsString(row);
            ++categoryCounts[category];
        }
    }

    if (categoryCounts.empty()) {
        throw EmptyColumn("Bar chart requires at least one non-missing value");
    }

    for (const auto& category : categoryCounts) {
        output << category.first << " | ";
        for (size_t mark = 0; mark < category.second; ++mark) {
            output << '#';
        }
        output << ' ' << category.second << '\n';
    }
}

} // namespace dal
