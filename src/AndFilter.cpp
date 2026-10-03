#include "dal/Filters.h"

#include <stdexcept>
#include <utility>

namespace dal {

AndFilter::AndFilter(std::unique_ptr<IFilter> left, std::unique_ptr<IFilter> right)
    : left_(std::move(left)), right_(std::move(right)) {}
bool AndFilter::matches(const DataSet&, std::size_t) const { throw std::logic_error("not implemented"); }
std::unique_ptr<IFilter> AndFilter::clone() const { throw std::logic_error("not implemented"); }

} // namespace dal
