#include "dal/IO.h"

#include <stdexcept>

namespace dal {
void CsvExporter::save(const DataSet&, const std::string&) { throw std::logic_error("not implemented"); }
} // namespace dal
