#include "dal/Filters.h"

#include <stdexcept>
#include <utility>
using namespace std;

namespace dal {

NotFilter::NotFilter(unique_ptr<IFilter> child)
    : child_(move(child)) {
    if (!child_) {
        throw invalid_argument("NotFilter requires a child");
    }
}

bool NotFilter::matches(const DataSet& data, size_t row) const {
    return !child_->matches(data, row);
}

unique_ptr<IFilter> NotFilter::clone() const {
    return make_unique<NotFilter>(child_->clone());
}

} // namespace dal
