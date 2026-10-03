#pragma once

#include "dal/ColumnBase.h"
#include "dal/exceptions.h"

#include <optional>
#include <iostream>
#include <algorithm>
#include <functional>
#include <sstream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace dal {

// Demonstrates generic programming, encapsulation, and overriding a type-erased interface.
template <typename T>
class Column final : public ColumnBase {
public:
    explicit Column(std::string name) : name_(std::move(name)) {}
    Column(std::string name, std::vector<std::optional<T>> data)
        : name_(std::move(name)), data_(std::move(data)) {}

    const std::string& getName() const override { return name_; }
    std::size_t size() const override { return data_.size(); }
    void print() const override {
        for (std::size_t i = 0; i < data_.size(); ++i) {
            if (i != 0U) { std::cout << ' '; }
            if (data_[i].has_value()) { std::cout << *data_[i]; }
            else { std::cout << "NA"; }
        }
        std::cout << '\n';
    }
    std::unique_ptr<ColumnBase> clone() const override {
        return std::make_unique<Column<T>>(*this);
    }
    bool isNumeric() const override { return std::is_arithmetic<T>::value && !std::is_same<T, bool>::value; }
    double toDouble(std::size_t i) const override {
        if constexpr (std::is_arithmetic<T>::value) {
            if (i >= data_.size() || !data_[i].has_value()) {
                throw std::out_of_range("Column index is out of range or missing");
            }
            return static_cast<double>(*data_[i]);
        } else {
            (void)i;
            throw TypeMismatch("Cannot convert " + typeName() + " column to double");
        }
    }
    bool isMissing(std::size_t i) const override {
        if (i >= data_.size()) { throw std::out_of_range("Column index out of range"); }
        return !data_[i].has_value();
    }
    std::string typeName() const override {
        if constexpr (std::is_same<T, int>::value) { return "int"; }
        else if constexpr (std::is_same<T, double>::value) { return "double"; }
        else if constexpr (std::is_same<T, std::string>::value) { return "string"; }
        else { return "unknown"; }
    }
    std::unique_ptr<ColumnBase> copyRows(const std::vector<std::size_t>& rows) const override {
        std::vector<std::optional<T>> values;
        values.reserve(rows.size());
        for (const auto row : rows) { values.push_back(data_.at(row)); }
        return std::make_unique<Column<T>>(name_, std::move(values));
    }
    std::string valueAsString(std::size_t i) const override {
        const auto& value = data_.at(i);
        if (!value) { return "NA"; }
        std::ostringstream output;
        output << *value;
        return output.str();
    }
    void addValue(const T& value) { data_.emplace_back(value); }
    void addMissing() { data_.emplace_back(std::nullopt); }
    const std::optional<T>& at(std::size_t i) const { return data_.at(i); }
    void set(std::size_t i, const T& value) { data_.at(i) = value; }
    const std::vector<std::optional<T>>& getData() const { return data_; }
    std::size_t countMissing() const {
        return static_cast<std::size_t>(std::count_if(data_.begin(), data_.end(),
            [](const auto& value) { return !value.has_value(); }));
    }
    Column<T> sorted() const {
        auto values = data_;
        std::stable_sort(values.begin(), values.end(), [](const auto& left, const auto& right) {
            if (!left) { return false; }
            if (!right) { return true; }
            return *left < *right;
        });
        return Column<T>(name_, std::move(values));
    }
    Column<T> slice(std::size_t from, std::size_t to) const {
        if (from > to || to > data_.size()) { throw std::out_of_range("Invalid column slice"); }
        return Column<T>(name_, std::vector<std::optional<T>>(data_.begin() + static_cast<std::ptrdiff_t>(from),
                                                               data_.begin() + static_cast<std::ptrdiff_t>(to)));
    }
    Column<T> unique() const {
        std::vector<std::optional<T>> values;
        for (const auto& value : data_) {
            if (std::find(values.begin(), values.end(), value) == values.end()) { values.push_back(value); }
        }
        return Column<T>(name_, std::move(values));
    }

    template <typename U = T, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Column<T> operator+(const Column<T>& rhs) const { return binary(rhs, std::plus<T>{}); }
    template <typename U = T, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Column<T> operator-(const Column<T>& rhs) const { return binary(rhs, std::minus<T>{}); }
    template <typename U = T, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Column<T> operator*(const Column<T>& rhs) const { return binary(rhs, std::multiplies<T>{}); }
    template <typename U = T, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Column<T> operator/(const Column<T>& rhs) const { return binary(rhs, std::divides<T>{}); }
    template <typename U = T, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Column<T> operator+(T rhs) const { return scalar(rhs, std::plus<T>{}); }
    template <typename U = T, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Column<T> operator-(T rhs) const { return scalar(rhs, std::minus<T>{}); }
    template <typename U = T, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Column<T> operator*(T rhs) const { return scalar(rhs, std::multiplies<T>{}); }
    template <typename U = T, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    Column<T> operator/(T rhs) const { return scalar(rhs, std::divides<T>{}); }
    Column<bool> operator==(const Column<T>& rhs) const { return compare(rhs, std::equal_to<T>{}); }
    Column<bool> operator!=(const Column<T>& rhs) const { return compare(rhs, std::not_equal_to<T>{}); }
    Column<bool> operator<(const Column<T>& rhs) const { return compare(rhs, std::less<T>{}); }
    Column<bool> operator<=(const Column<T>& rhs) const { return compare(rhs, std::less_equal<T>{}); }
    Column<bool> operator>(const Column<T>& rhs) const { return compare(rhs, std::greater<T>{}); }
    Column<bool> operator>=(const Column<T>& rhs) const { return compare(rhs, std::greater_equal<T>{}); }
    Column<bool> operator==(const T& rhs) const { return compareScalar(rhs, std::equal_to<T>{}); }
    Column<bool> operator!=(const T& rhs) const { return compareScalar(rhs, std::not_equal_to<T>{}); }
    Column<bool> operator<(const T& rhs) const { return compareScalar(rhs, std::less<T>{}); }
    Column<bool> operator<=(const T& rhs) const { return compareScalar(rhs, std::less_equal<T>{}); }
    Column<bool> operator>(const T& rhs) const { return compareScalar(rhs, std::greater<T>{}); }
    Column<bool> operator>=(const T& rhs) const { return compareScalar(rhs, std::greater_equal<T>{}); }

private:
    template <typename Op> Column<T> binary(const Column<T>& rhs, Op op) const {
        if (size() != rhs.size()) { throw std::invalid_argument("Column sizes differ"); }
        std::vector<std::optional<T>> values(size());
        for (std::size_t i = 0; i < size(); ++i) {
            if (data_[i] && rhs.data_[i]) { values[i] = op(*data_[i], *rhs.data_[i]); }
        }
        return Column<T>(name_, std::move(values));
    }
    template <typename Op> Column<T> scalar(T rhs, Op op) const {
        std::vector<std::optional<T>> values(size());
        for (std::size_t i = 0; i < size(); ++i) { if (data_[i]) { values[i] = op(*data_[i], rhs); } }
        return Column<T>(name_, std::move(values));
    }
    template <typename Op> Column<bool> compare(const Column<T>& rhs, Op op) const {
        if (size() != rhs.size()) { throw std::invalid_argument("Column sizes differ"); }
        std::vector<std::optional<bool>> values(size());
        for (std::size_t i = 0; i < size(); ++i) {
            if (data_[i] && rhs.data_[i]) { values[i] = op(*data_[i], *rhs.data_[i]); }
        }
        return Column<bool>(name_ + "_mask", std::move(values));
    }
    template <typename Op> Column<bool> compareScalar(const T& rhs, Op op) const {
        std::vector<std::optional<bool>> values(size());
        for (std::size_t i = 0; i < size(); ++i) { if (data_[i]) { values[i] = op(*data_[i], rhs); } }
        return Column<bool>(name_ + "_mask", std::move(values));
    }
    std::string name_;
    std::vector<std::optional<T>> data_;
};

} // namespace dal
