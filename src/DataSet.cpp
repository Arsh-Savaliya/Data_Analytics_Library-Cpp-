#include "dal/DataSet.h"
#include "dal/IFilter.h"
#include "dal/exceptions.h"

#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>

namespace dal {

DataSet::DataSet(const DataSet& other) {
    for (const auto& column : other.columns_) { columns_.push_back(column->clone()); }
}

DataSet& DataSet::operator=(const DataSet& other) {
    if (this != &other) {
        DataSet copy(other);
        columns_ = std::move(copy.columns_);
    }
    return *this;
}

void DataSet::addColumn(std::unique_ptr<ColumnBase> column) {
    if (!column) { throw std::invalid_argument("Cannot add a null column"); }
    for (const auto& current : columns_) {
        if (current->getName() == column->getName()) { throw std::invalid_argument("Duplicate column: " + column->getName()); }
    }
    if (!columns_.empty() && column->size() != rowCount()) { throw std::invalid_argument("Column row count does not match DataSet"); }
    columns_.push_back(std::move(column));
}
void DataSet::removeColumn(const std::string& name) {
    const auto it = std::find_if(columns_.begin(), columns_.end(), [&name](const auto& column) { return column->getName() == name; });
    if (it == columns_.end()) { throw ColumnNotFound("Column not found: " + name); }
    columns_.erase(it);
}
ColumnBase& DataSet::getColumn(const std::string& name) {
    for (auto& column : columns_) { if (column->getName() == name) { return *column; } }
    throw ColumnNotFound("Column not found: " + name);
}
const ColumnBase& DataSet::getColumn(const std::string& name) const {
    for (const auto& column : columns_) { if (column->getName() == name) { return *column; } }
    throw ColumnNotFound("Column not found: " + name);
}
std::size_t DataSet::columnCount() const { return columns_.size(); }
std::size_t DataSet::rowCount() const { return columns_.empty() ? 0U : columns_.front()->size(); }
std::vector<std::string> DataSet::columnNames() const {
    std::vector<std::string> names;
    names.reserve(columns_.size());
    for (const auto& column : columns_) { names.push_back(column->getName()); }
    return names;
}
DataSet DataSet::filterBy(const IFilter& filter) const {
    std::vector<std::size_t> selected;
    for (std::size_t row = 0; row < rowCount(); ++row) { if (filter.matches(*this, row)) { selected.push_back(row); } }
    DataSet result;
    for (const auto& column : columns_) { result.addColumn(column->copyRows(selected)); }
    return result;
}
DataSet DataSet::select(const std::vector<std::string>& names) const {
    DataSet result;
    for (const auto& name : names) { result.addColumn(getColumn(name).clone()); }
    return result;
}
DataSet DataSet::head(std::size_t n) const {
    const auto count = std::min(n, rowCount());
    std::vector<std::size_t> rows(count);
    std::iota(rows.begin(), rows.end(), 0U);
    DataSet result;
    for (const auto& column : columns_) { result.addColumn(column->copyRows(rows)); }
    return result;
}
DataSet DataSet::tail(std::size_t n) const {
    const auto count = std::min(n, rowCount());
    std::vector<std::size_t> rows(count);
    const auto start = rowCount() - count;
    std::iota(rows.begin(), rows.end(), start);
    DataSet result;
    for (const auto& column : columns_) { result.addColumn(column->copyRows(rows)); }
    return result;
}
void DataSet::printTable() const {
    std::vector<std::size_t> widths;
    for (const auto& column : columns_) {
        std::size_t width = column->getName().size();
        for (std::size_t row = 0; row < column->size(); ++row) { width = std::max(width, column->valueAsString(row).size()); }
        widths.push_back(width);
    }
    for (std::size_t c = 0; c < columns_.size(); ++c) { std::cout << std::left << std::setw(static_cast<int>(widths[c] + 2U)) << columns_[c]->getName(); }
    std::cout << '\n';
    for (std::size_t row = 0; row < rowCount(); ++row) {
        for (std::size_t c = 0; c < columns_.size(); ++c) { std::cout << std::left << std::setw(static_cast<int>(widths[c] + 2U)) << columns_[c]->valueAsString(row); }
        std::cout << '\n';
    }
}

} // namespace dal
