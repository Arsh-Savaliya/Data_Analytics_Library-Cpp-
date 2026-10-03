#include "dal/CountAnalyzer.h"
#include "dal/exceptions.h"
using namespace std;

namespace dal {
double CountAnalyzer::analyze(const ColumnBase& column) const {
    if (!column.isNumeric()) {
        throw TypeMismatch("Count requires a numeric column");
    }
    size_t count = 0U;
    for (size_t i = 0; i < column.size(); ++i) {
        if (!column.isMissing(i)) {
            ++count;
        }
    }
    if (count == 0U) {
        throw EmptyColumn("Count requires at least one non-missing value");
    }
    return static_cast<double>(count);
}

string CountAnalyzer::name() const {
    return "count";
}
}
