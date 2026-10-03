#pragma once
#include "dal/ColumnBase.h"

#include <ostream>
#include <cstddef>
#include <vector>
using namespace std;

namespace dal {

// Strategy interface for interchangeable visualization behavior, dispatched polymorphically.
class Visualizer {
public:
    virtual ~Visualizer() = default;
    virtual void render(const ColumnBase& column, ostream& output) const = 0;
};

// Histogram visualization strategy with configurable bin count.
class HistogramViz final : public Visualizer {
public:
    explicit HistogramViz(size_t bins = 10U);
    void render(const ColumnBase& column, ostream& output) const override;
private:
    size_t bins_;
};

// Category-count visualization strategy for string-like columns.
class BarChartViz final : public Visualizer {
public:
    void render(const ColumnBase& column, ostream& output) const override;
};

// Five-number summary visualization strategy for numeric columns.
class BoxSummaryViz final : public Visualizer {
public:
    void render(const ColumnBase& column, ostream& output) const override;
private:
    double quantile(const vector<double>& sortedValues, double percentage) const;
};

} // namespace dal
