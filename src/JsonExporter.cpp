#include "dal/IO.h"

#include <stdexcept>

namespace dal {
void JsonExporter::save(const DataSet&, const std::string&) { throw std::logic_error("not implemented"); }
} // namespace dal
