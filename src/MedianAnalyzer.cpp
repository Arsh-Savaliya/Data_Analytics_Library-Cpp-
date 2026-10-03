#include "dal/IAnalyzer.h"
#include <stdexcept>

namespace dal {

double MedianAnalyzer::analyze(const ColumnBase&) const { throw std::logic_error("not implemented"); }
std::string MedianAnalyzer::name() const { return "median"; }

} // namespace dal
