#pragma once

#include "dal/ColumnBase.h"
#include "dal/exceptions.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iomanip>
#include <limits>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
using namespace std;

namespace dal {

// Demonstrates generic programming, encapsulation, and overriding a type-erased interface.
template <typename T>
class Column final : public ColumnBase {
public:
    explicit Column(string name) : name_(move(name)) {}
    Column(string name, vector<optional<T>> data)
        : name_(move(name)), data_(move(data)) {}

    const string& getName() const override { return name_; }
    size_t size() const override { return data_.size(); }
    void print() const override {
        for (size_t i = 0; i < data_.size(); ++i) {
            if (i != 0U) { cout << ' '; }
            if (data_[i].has_value()) { cout << *data_[i]; }
            else { cout << "NA"; }
        }
        cout << '\n';
    }
    unique_ptr<ColumnBase> clone() const override {
        return make_unique<Column<T>>(*this);
    }
    bool isNumeric() const override { return is_arithmetic<T>::value && !is_same<T, bool>::value; }
    double toDouble(size_t i) const override {
        if constexpr (is_arithmetic<T>::value && !is_same<T, bool>::value) {
            if (i >= data_.size() || !data_[i].has_value()) {
                throw out_of_range("Column index is out of range or missing");
            }
            return static_cast<double>(*data_[i]);
        } else {
            (void)i;
            throw TypeMismatch("Cannot convert " + typeName() + " column to double");
        }
    }
    bool isMissing(size_t i) const override {
        if (i >= data_.size()) { throw out_of_range("Column index out of range"); }
        return !data_[i].has_value();
    }
    string typeName() const override {
        if constexpr (is_same<T, int>::value) { return "int"; }
        else if constexpr (is_same<T, double>::value) { return "double"; }
        else if constexpr (is_same<T, string>::value) { return "string"; }
        else { return "unknown"; }
    }
    unique_ptr<ColumnBase> copyRows(const vector<size_t>& rows) const override {
        vector<optional<T>> values;
        values.reserve(rows.size());
        for (const auto row : rows) { values.push_back(data_.at(row)); }
        return make_unique<Column<T>>(name_, move(values));
    }
    string valueAsString(size_t i) const override {
        const auto& value = data_.at(i);
        if (!value) { return "NA"; }
        ostringstream output;
        output << *value;
        return output.str();
    }
    string keyAt(size_t i) const override {
        const auto& value = data_.at(i);
        if (!value) {
            return string();
        }
        ostringstream output;
        if constexpr (is_same<T, double>::value) {
            output << setprecision(numeric_limits<double>::max_digits10);
        }
        output << *value;
        return output.str();
    }
    unique_ptr<ColumnBase> cloneWithName(const string& newName) const override {
        return make_unique<Column<T>>(newName, data_);
    }
    void addValue(const T& value) { data_.emplace_back(value); }
    void addMissing() { data_.emplace_back(nullopt); }
    const optional<T>& at(size_t i) const { return data_.at(i); }
    void set(size_t i, const T& value) { data_.at(i) = value; }
    const vector<optional<T>>& getData() const { return data_; }
    size_t countMissing() const {
        return static_cast<size_t>(count_if(data_.begin(), data_.end(),
            [](const auto& value) { return !value.has_value(); }));
    }
    Column<T> sorted() const {
        auto values = data_;
        stable_sort(values.begin(), values.end(), [](const auto& left, const auto& right) {
            if (!left) { return false; }
            if (!right) { return true; }
            return *left < *right;
        });
        return Column<T>(name_, move(values));
    }
    Column<T> slice(size_t from, size_t to) const {
        if (from > to || to > data_.size()) { throw out_of_range("Invalid column slice"); }
        return Column<T>(name_, vector<optional<T>>(data_.begin() + static_cast<ptrdiff_t>(from),
                                                               data_.begin() + static_cast<ptrdiff_t>(to)));
    }
    Column<T> unique() const {
        vector<optional<T>> values;
        for (const auto& value : data_) {
            if (find(values.begin(), values.end(), value) == values.end()) { values.push_back(value); }
        }
        return Column<T>(name_, move(values));
    }

    template <typename U = T, typename = enable_if_t<is_arithmetic<U>::value>>
    Column<T> operator+(const Column<T>& rhs) const { return binary(rhs, plus<T>{}); }
    template <typename U = T, typename = enable_if_t<is_arithmetic<U>::value>>
    Column<T> operator-(const Column<T>& rhs) const { return binary(rhs, minus<T>{}); }
    template <typename U = T, typename = enable_if_t<is_arithmetic<U>::value>>
    Column<T> operator*(const Column<T>& rhs) const { return binary(rhs, multiplies<T>{}); }
    template <typename U = T, typename = enable_if_t<is_arithmetic<U>::value>>
    Column<T> operator/(const Column<T>& rhs) const { return divideByColumn(rhs); }
    template <typename U = T, typename = enable_if_t<is_arithmetic<U>::value>>
    Column<T> operator+(T rhs) const { return scalar(rhs, plus<T>{}); }
    template <typename U = T, typename = enable_if_t<is_arithmetic<U>::value>>
    Column<T> operator-(T rhs) const { return scalar(rhs, minus<T>{}); }
    template <typename U = T, typename = enable_if_t<is_arithmetic<U>::value>>
    Column<T> operator*(T rhs) const { return scalar(rhs, multiplies<T>{}); }
    template <typename U = T, typename = enable_if_t<is_arithmetic<U>::value>>
    Column<T> operator/(T rhs) const { return divideByScalar(rhs); }
    Column<bool> operator==(const Column<T>& rhs) const { return compare(rhs, equal_to<T>{}); }
    Column<bool> operator!=(const Column<T>& rhs) const { return compare(rhs, not_equal_to<T>{}); }
    Column<bool> operator<(const Column<T>& rhs) const { return compare(rhs, less<T>{}); }
    Column<bool> operator<=(const Column<T>& rhs) const { return compare(rhs, less_equal<T>{}); }
    Column<bool> operator>(const Column<T>& rhs) const { return compare(rhs, greater<T>{}); }
    Column<bool> operator>=(const Column<T>& rhs) const { return compare(rhs, greater_equal<T>{}); }
    Column<bool> operator==(const T& rhs) const { return compareScalar(rhs, equal_to<T>{}); }
    Column<bool> operator!=(const T& rhs) const { return compareScalar(rhs, not_equal_to<T>{}); }
    Column<bool> operator<(const T& rhs) const { return compareScalar(rhs, less<T>{}); }
    Column<bool> operator<=(const T& rhs) const { return compareScalar(rhs, less_equal<T>{}); }
    Column<bool> operator>(const T& rhs) const { return compareScalar(rhs, greater<T>{}); }
    Column<bool> operator>=(const T& rhs) const { return compareScalar(rhs, greater_equal<T>{}); }

private:
    Column<T> divideByColumn(const Column<T>& rhs) const {
        if (size() != rhs.size()) {
            throw invalid_argument("Column sizes differ");
        }

        vector<optional<T>> values(size());
        for (size_t i = 0; i < size(); ++i) {
            if (!data_[i] || !rhs.data_[i]) {
                continue;
            }
            if (*rhs.data_[i] == T{}) {
                throw domain_error("Cannot divide a column value by zero");
            }
            if constexpr (is_integral<T>::value && !is_same<T, bool>::value) {
                if (*data_[i] == numeric_limits<T>::min() && *rhs.data_[i] == T(-1)) {
                    throw overflow_error("Integer division result is out of range");
                }
            }
            values[i] = *data_[i] / *rhs.data_[i];
        }
        return Column<T>(name_, move(values));
    }

    Column<T> divideByScalar(T divisor) const {
        if (divisor == T{}) {
            throw domain_error("Cannot divide a column by zero");
        }

        vector<optional<T>> values(size());
        for (size_t i = 0; i < size(); ++i) {
            if (data_[i]) {
                if constexpr (is_integral<T>::value && !is_same<T, bool>::value) {
                    if (*data_[i] == numeric_limits<T>::min() && divisor == T(-1)) {
                        throw overflow_error("Integer division result is out of range");
                    }
                }
                values[i] = *data_[i] / divisor;
            }
        }
        return Column<T>(name_, move(values));
    }

    template <typename Op> Column<T> binary(const Column<T>& rhs, Op op) const {
        if (size() != rhs.size()) { throw invalid_argument("Column sizes differ"); }
        vector<optional<T>> values(size());
        for (size_t i = 0; i < size(); ++i) {
            if (data_[i] && rhs.data_[i]) {
                values[i] = applyArithmetic(*data_[i], *rhs.data_[i], op);
            }
        }
        return Column<T>(name_, move(values));
    }
    template <typename Op> Column<T> scalar(T rhs, Op op) const {
        vector<optional<T>> values(size());
        for (size_t i = 0; i < size(); ++i) {
            if (data_[i]) {
                values[i] = applyArithmetic(*data_[i], rhs, op);
            }
        }
        return Column<T>(name_, move(values));
    }
    template <typename Op> Column<bool> compare(const Column<T>& rhs, Op op) const {
        if (size() != rhs.size()) { throw invalid_argument("Column sizes differ"); }
        vector<optional<bool>> values(size());
        for (size_t i = 0; i < size(); ++i) {
            if (data_[i] && rhs.data_[i]) { values[i] = op(*data_[i], *rhs.data_[i]); }
        }
        return Column<bool>(name_ + "_mask", move(values));
    }
    template <typename Op> Column<bool> compareScalar(const T& rhs, Op op) const {
        vector<optional<bool>> values(size());
        for (size_t i = 0; i < size(); ++i) { if (data_[i]) { values[i] = op(*data_[i], rhs); } }
        return Column<bool>(name_ + "_mask", move(values));
    }

    template <typename Op>
    T applyArithmetic(T left, T right, Op operation) const {
        if constexpr (is_same<T, int>::value) {
            long long result = 0;
            if constexpr (is_same<Op, plus<T>>::value) {
                result = static_cast<long long>(left) + right;
            } else if constexpr (is_same<Op, minus<T>>::value) {
                result = static_cast<long long>(left) - right;
            } else if constexpr (is_same<Op, multiplies<T>>::value) {
                result = static_cast<long long>(left) * right;
            } else {
                return operation(left, right);
            }

            if (result < numeric_limits<int>::min() || result > numeric_limits<int>::max()) {
                throw overflow_error("Integer column arithmetic result is out of range");
            }
            return static_cast<int>(result);
        }

        return operation(left, right);
    }
    string name_;
    vector<optional<T>> data_;
};

} // namespace dal
