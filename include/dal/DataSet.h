#pragma once

#include "dal/ColumnBase.h"
#include "dal/Column.h"
#include "dal/exceptions.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace dal {
class IFilter;

// Demonstrates composition and ownership through a collection of polymorphic columns.
class DataSet {
public:
    DataSet() = default;
    DataSet(const DataSet& other);
    DataSet& operator=(const DataSet& other);
    DataSet(DataSet&&) noexcept = default;
    DataSet& operator=(DataSet&&) noexcept = default;
    ~DataSet() = default;

    void addColumn(std::unique_ptr<ColumnBase> column);
    void removeColumn(const std::string& name);
    ColumnBase& getColumn(const std::string& name);
    const ColumnBase& getColumn(const std::string& name) const;
    template <typename T> Column<T>& getColumnAs(const std::string& name) {
        auto* typed = dynamic_cast<Column<T>*>(&getColumn(name));
        if (typed == nullptr) { throw TypeMismatch("Column '" + name + "' has type " + getColumn(name).typeName()); }
        return *typed;
    }
    template <typename T> const Column<T>& getColumnAs(const std::string& name) const {
        const auto* typed = dynamic_cast<const Column<T>*>(&getColumn(name));
        if (typed == nullptr) { throw TypeMismatch("Column '" + name + "' has type " + getColumn(name).typeName()); }
        return *typed;
    }
    std::size_t columnCount() const;
    std::size_t rowCount() const;
    std::vector<std::string> columnNames() const;
    DataSet filterBy(const IFilter& filter) const;
    DataSet select(const std::vector<std::string>& names) const;
    DataSet head(std::size_t n) const;
    DataSet tail(std::size_t n) const;
    void printTable() const;

private:
    std::vector<std::unique_ptr<ColumnBase>> columns_;
};

} // namespace dal
