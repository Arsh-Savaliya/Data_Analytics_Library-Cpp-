#include "dal/DataSet.h"
#include "dal/IFilter.h"
#include "dal/GroupedDataSet.h"
#include "dal/exceptions.h"

#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <map>
#include <set>
using namespace std;

namespace dal {

DataSet::DataSet(const DataSet& other)
    : rowCount_(other.rowCount_),
      rowCountIsKnown_(other.rowCountIsKnown_) {
    for (const auto& column : other.columns_) { columns_.push_back(column->clone()); }
}

DataSet& DataSet::operator=(const DataSet& other) {
    if (this != &other) {
        DataSet copy(other);
        columns_ = move(copy.columns_);
        rowCount_ = copy.rowCount_;
        rowCountIsKnown_ = copy.rowCountIsKnown_;
    }
    return *this;
}

DataSet::DataSet(DataSet&& other) noexcept
    : columns_(move(other.columns_)), rowCount_(other.rowCount_),
      rowCountIsKnown_(other.rowCountIsKnown_) {
    other.rowCount_ = 0U;
    other.rowCountIsKnown_ = false;
}

DataSet& DataSet::operator=(DataSet&& other) noexcept {
    if (this != &other) {
        columns_ = move(other.columns_);
        rowCount_ = other.rowCount_;
        rowCountIsKnown_ = other.rowCountIsKnown_;
        other.rowCount_ = 0U;
        other.rowCountIsKnown_ = false;
    }
    return *this;
}

DataSet::~DataSet() = default;

void DataSet::addColumn(unique_ptr<ColumnBase> column) {
    if (!column) { throw invalid_argument("Cannot add a null column"); }
    for (const auto& current : columns_) {
        if (current->getName() == column->getName()) { throw invalid_argument("Duplicate column: " + column->getName()); }
    }
    if (rowCountIsKnown_ && column->size() != rowCount_) { throw invalid_argument("Column row count does not match DataSet"); }
    if (!rowCountIsKnown_) {
        rowCount_ = column->size();
        rowCountIsKnown_ = true;
    }
    columns_.push_back(move(column));
}
void DataSet::removeColumn(const string& name) {
    const auto it = find_if(columns_.begin(), columns_.end(), [&name](const auto& column) { return column->getName() == name; });
    if (it == columns_.end()) { throw ColumnNotFound("Column not found: " + name); }
    columns_.erase(it);
}
ColumnBase& DataSet::getColumn(const string& name) {
    for (auto& column : columns_) { if (column->getName() == name) { return *column; } }
    throw ColumnNotFound("Column not found: " + name);
}
const ColumnBase& DataSet::getColumn(const string& name) const {
    for (const auto& column : columns_) { if (column->getName() == name) { return *column; } }
    throw ColumnNotFound("Column not found: " + name);
}
size_t DataSet::columnCount() const { return columns_.size(); }
size_t DataSet::rowCount() const { return rowCount_; }
vector<string> DataSet::columnNames() const {
    vector<string> names;
    names.reserve(columns_.size());
    for (const auto& column : columns_) { names.push_back(column->getName()); }
    return names;
}
DataSet DataSet::filterBy(const IFilter& filter) const {
    vector<size_t> selected;
    for (size_t row = 0; row < rowCount(); ++row) { if (filter.matches(*this, row)) { selected.push_back(row); } }
    DataSet result;
    result.rowCount_ = selected.size();
    result.rowCountIsKnown_ = true;
    for (const auto& column : columns_) { result.addColumn(column->copyRows(selected)); }
    return result;
}
DataSet DataSet::select(const vector<string>& names) const {
    DataSet result;
    result.rowCount_ = rowCount();
    result.rowCountIsKnown_ = true;
    for (const auto& name : names) { result.addColumn(getColumn(name).clone()); }
    return result;
}
DataSet DataSet::head(size_t n) const {
    const auto count = min(n, rowCount());
    vector<size_t> rows(count);
    iota(rows.begin(), rows.end(), 0U);
    DataSet result;
    result.rowCount_ = count;
    result.rowCountIsKnown_ = true;
    for (const auto& column : columns_) { result.addColumn(column->copyRows(rows)); }
    return result;
}
DataSet DataSet::tail(size_t n) const {
    const auto count = min(n, rowCount());
    vector<size_t> rows(count);
    const auto start = rowCount() - count;
    iota(rows.begin(), rows.end(), start);
    DataSet result;
    result.rowCount_ = count;
    result.rowCountIsKnown_ = true;
    for (const auto& column : columns_) { result.addColumn(column->copyRows(rows)); }
    return result;
}
DataSet DataSet::sortBy(const string& columnName, bool ascending) const {
    const auto& key = getColumn(columnName);
    vector<size_t> rows(rowCount());
    iota(rows.begin(), rows.end(), 0U);
    stable_sort(rows.begin(), rows.end(), [&](size_t left, size_t right) {
        const bool leftMissing = key.isMissing(left);
        const bool rightMissing = key.isMissing(right);
        if (leftMissing != rightMissing) { return !leftMissing; }
        if (leftMissing) { return false; }
        bool less = false;
        if (key.isNumeric()) { less = key.toDouble(left) < key.toDouble(right); }
        else { less = key.keyAt(left) < key.keyAt(right); }
        return ascending ? less : (key.isNumeric() ? key.toDouble(left) > key.toDouble(right)
                                                    : key.keyAt(left) > key.keyAt(right));
    });
    DataSet result;
    result.rowCount_ = rows.size();
    result.rowCountIsKnown_ = true;
    for (const auto& column : columns_) { result.addColumn(column->copyRows(rows)); }
    return result;
}

DataSet DataSet::join(const DataSet& other, const string& keyName) const {
    const auto& leftKey = getColumn(keyName);
    const auto& rightKey = other.getColumn(keyName);
    if (leftKey.typeName() != rightKey.typeName()) {
        throw TypeMismatch("Join key types do not match");
    }
    map<string, vector<size_t>> rightRowsByKey;
    for (size_t row = 0; row < other.rowCount(); ++row) {
        if (!rightKey.isMissing(row)) { rightRowsByKey[rightKey.keyAt(row)].push_back(row); }
    }
    vector<size_t> leftRows;
    vector<size_t> rightRows;
    for (size_t row = 0; row < rowCount(); ++row) {
        if (leftKey.isMissing(row)) { continue; }
        const auto found = rightRowsByKey.find(leftKey.keyAt(row));
        if (found != rightRowsByKey.end()) {
            for (const size_t match : found->second) { leftRows.push_back(row); rightRows.push_back(match); }
        }
    }
    DataSet result;
    result.rowCount_ = leftRows.size();
    result.rowCountIsKnown_ = true;
    set<string> names;
    for (const auto& column : columns_) {
        result.addColumn(column->copyRows(leftRows));
        names.insert(column->getName());
    }
    for (const auto& column : other.columns_) {
        if (column->getName() == keyName) { continue; }
        string outputName = column->getName();
        if (names.count(outputName) != 0U) { outputName = "right_" + outputName; }
        while (names.count(outputName) != 0U) { outputName = "right_" + outputName; }
        names.insert(outputName);
        result.addColumn(column->copyRows(rightRows)->cloneWithName(outputName));
    }
    return result;
}

GroupedDataSet DataSet::groupBy(const string& columnName) const {
    return GroupedDataSet(*this, columnName);
}

void DataSet::printTable() const {
    printTable(cout);
}
void DataSet::printTable(ostream& output) const {
    vector<size_t> widths;
    for (const auto& column : columns_) {
        size_t width = column->getName().size();
        for (size_t row = 0; row < column->size(); ++row) { width = max(width, column->valueAsString(row).size()); }
        widths.push_back(width);
    }
    for (size_t c = 0; c < columns_.size(); ++c) { output << left << setw(static_cast<int>(widths[c] + 2U)) << columns_[c]->getName(); }
    output << '\n';
    for (size_t row = 0; row < rowCount(); ++row) {
        for (size_t c = 0; c < columns_.size(); ++c) { output << left << setw(static_cast<int>(widths[c] + 2U)) << columns_[c]->valueAsString(row); }
        output << '\n';
    }
}

} // namespace dal
