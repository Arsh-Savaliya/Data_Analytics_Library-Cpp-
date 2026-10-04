#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>
using namespace std;

namespace dal {

// Demonstrates abstraction and runtime polymorphism for type-erased columns.
class ColumnBase {
public:
    virtual ~ColumnBase() = default;
    virtual const string& getName() const = 0;
    virtual size_t size() const = 0;
    virtual void print() const = 0;
    virtual unique_ptr<ColumnBase> clone() const = 0;
    virtual bool isNumeric() const = 0;
    virtual double toDouble(size_t i) const = 0;
    virtual bool isMissing(size_t i) const = 0;
    virtual string typeName() const = 0;
    // Runtime dispatch preserves each concrete column's value type during row transformations.
    virtual unique_ptr<ColumnBase> copyRows(const vector<size_t>& rows) const = 0;
    virtual string valueAsString(size_t i) const = 0;
    // keyAt gives grouping and joining a precise representation of a stored value.
    virtual string keyAt(size_t i) const = 0;
    virtual unique_ptr<ColumnBase> cloneWithName(const string& newName) const = 0;
};

} // namespace dal
