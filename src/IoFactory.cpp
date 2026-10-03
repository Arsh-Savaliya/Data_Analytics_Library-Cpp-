#include "dal/IO.h"

#include <stdexcept>

namespace dal {
std::unique_ptr<IImporter> IoFactory::makeImporter(const std::string&) { throw std::logic_error("not implemented"); }
std::unique_ptr<IExporter> IoFactory::makeExporter(const std::string&) { throw std::logic_error("not implemented"); }
} // namespace dal
