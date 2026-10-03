#include "dal/Visualizer.h"

#include <stdexcept>

namespace dal {
void HistogramViz::render(const ColumnBase&, std::ostream&) const { throw std::logic_error("not implemented"); }
} // namespace dal
