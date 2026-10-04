#include "dal/Filters.h"

#include <stdexcept>
#include <utility>
using namespace std;

namespace dal {

AndFilter::AndFilter(unique_ptr<IFilter> left, unique_ptr<IFilter> right)
    : left_(move(left)), right_(move(right)) {
    if (!left_ || !right_) {
        throw invalid_argument("AndFilter requires two children");
    }
}

bool AndFilter::matches(const DataSet& data, size_t row) const {
    const bool leftMatches = left_->matches(data, row);
    if (!leftMatches) {
        return false;
    }
    return right_->matches(data, row);
}

unique_ptr<IFilter> AndFilter::clone() const {
    return make_unique<AndFilter>(left_->clone(), right_->clone());
}

} // namespace dal
