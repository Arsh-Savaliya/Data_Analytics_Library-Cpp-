#include "dal/Filters.h"

#include "dal/DataSet.h"

#include <stdexcept>

namespace dal {

template <typename T>
bool ComparisonFilter<T>::matches(const DataSet&, std::size_t) const {
    throw std::logic_error("not implemented");
}

template class ComparisonFilter<int>;
template class ComparisonFilter<double>;
template class ComparisonFilter<std::string>;

} // namespace dal
