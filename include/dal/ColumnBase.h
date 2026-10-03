#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace dal {

// Demonstrates abstraction and runtime polymorphism for type-erased columns.
class ColumnBase {
public:
    virtual ~ColumnBase() = default;
    virtual const std::string& getName() const = 0;
    virtual std::size_t size() const = 0;
    virtual void print() const = 0;
    virtual std::unique_ptr<ColumnBase> clone() const = 0;
    virtual bool isNumeric() const = 0;
    virtual double toDouble(std::size_t i) const = 0;
    virtual bool isMissing(std::size_t i) const = 0;
    virtual std::string typeName() const = 0;
    // Runtime dispatch preserves each concrete column's value type during row transformations.
    virtual std::unique_ptr<ColumnBase> copyRows(const std::vector<std::size_t>& rows) const = 0;
    virtual std::string valueAsString(std::size_t i) const = 0;
};

} // namespace dal
