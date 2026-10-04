#include "dal/Filters.h"

#include <stdexcept>
#include <utility>
using namespace std;

namespace dal {

OrFilter::OrFilter(unique_ptr<IFilter> left, unique_ptr<IFilter> right)
    : left_(move(left)), right_(move(right)) {
    if (!left_ || !right_) {
        throw invalid_argument("OrFilter requires two children");
    }
}

bool OrFilter::matches(const DataSet& data, size_t row) const {
    const bool leftMatches = left_->matches(data, row);
    if (leftMatches) {
        return true;
    }
    return right_->matches(data, row);
}

unique_ptr<IFilter> OrFilter::clone() const {
    return make_unique<OrFilter>(left_->clone(), right_->clone());
}

} // namespace dal
