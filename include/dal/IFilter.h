#pragma once

#include <cstddef>
#include <memory>
using namespace std;

namespace dal {
class DataSet;

// Demonstrates abstraction and polymorphic row predicates.
class IFilter {
public:
    virtual ~IFilter() = default;
    virtual bool matches(const DataSet& data, size_t row) const = 0;
    virtual unique_ptr<IFilter> clone() const = 0;
};

} // namespace dal
