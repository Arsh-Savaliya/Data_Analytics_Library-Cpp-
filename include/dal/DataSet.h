#pragma once

#include "dal/ColumnBase.h"
#include "dal/Column.h"
#include "dal/exceptions.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>
using namespace std;

namespace dal {
class IFilter;
class GroupedDataSet;

// Demonstrates composition and ownership through a collection of polymorphic columns.
class DataSet {
public:
    DataSet() = default;
    DataSet(const DataSet& other);
    DataSet& operator=(const DataSet& other);
    DataSet(DataSet&& other) noexcept;
    DataSet& operator=(DataSet&& other) noexcept;
    ~DataSet();

    void addColumn(unique_ptr<ColumnBase> column);
    void removeColumn(const string& name);
    ColumnBase& getColumn(const string& name);
    const ColumnBase& getColumn(const string& name) const;
    template <typename T> Column<T>& getColumnAs(const string& name) {
        auto* typed = dynamic_cast<Column<T>*>(&getColumn(name));
        if (typed == nullptr) { throw TypeMismatch("Column '" + name + "' has type " + getColumn(name).typeName()); }
        return *typed;
    }
    template <typename T> const Column<T>& getColumnAs(const string& name) const {
        const auto* typed = dynamic_cast<const Column<T>*>(&getColumn(name));
        if (typed == nullptr) { throw TypeMismatch("Column '" + name + "' has type " + getColumn(name).typeName()); }
        return *typed;
    }
    size_t columnCount() const;
    size_t rowCount() const;
    vector<string> columnNames() const;
    DataSet filterBy(const IFilter& filter) const;
    DataSet select(const vector<string>& names) const;
    DataSet head(size_t n) const;
    DataSet tail(size_t n) const;
    DataSet sortBy(const string& columnName, bool ascending = true) const;
    DataSet join(const DataSet& other, const string& key) const;
    GroupedDataSet groupBy(const string& columnName) const;
    void printTable() const;
    void printTable(ostream& output) const;

private:
    vector<unique_ptr<ColumnBase>> columns_;
    size_t rowCount_ = 0U;
    bool rowCountIsKnown_ = false;
};

} // namespace dal
