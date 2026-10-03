#include "dal/Filters.h"

#include <stdexcept>
#include <utility>

namespace dal {

OrFilter::OrFilter(std::unique_ptr<IFilter> left, std::unique_ptr<IFilter> right)
    : left_(std::move(left)), right_(std::move(right)) {}
bool OrFilter::matches(const DataSet&, std::size_t) const { throw std::logic_error("not implemented"); }
std::unique_ptr<IFilter> OrFilter::clone() const { throw std::logic_error("not implemented"); }

} // namespace dal
