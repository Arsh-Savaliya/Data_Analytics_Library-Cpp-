#include "dal/IAnalyzer.h"
#include "dal/exceptions.h"
#include <cstddef>

namespace dal {

double MeanAnalyzer::analyze(const ColumnBase& column) const {
    if (!column.isNumeric()) { throw TypeMismatch("Mean requires a numeric column"); }
    double sum = 0.0;
    std::size_t count = 0U;
    for (std::size_t i = 0; i < column.size(); ++i) {
        if (!column.isMissing(i)) { sum += column.toDouble(i); ++count; }
    }
    if (count == 0U) { throw EmptyColumn("Mean requires at least one value"); }
    return sum / static_cast<double>(count);
}

std::string MeanAnalyzer::name() const { return "mean"; }

} // namespace dal
