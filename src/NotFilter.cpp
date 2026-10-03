#include "dal/Filters.h"

#include <stdexcept>
#include <utility>

namespace dal {

NotFilter::NotFilter(std::unique_ptr<IFilter> child) : child_(std::move(child)) {}
bool NotFilter::matches(const DataSet&, std::size_t) const { throw std::logic_error("not implemented"); }
std::unique_ptr<IFilter> NotFilter::clone() const { throw std::logic_error("not implemented"); }

} // namespace dal
