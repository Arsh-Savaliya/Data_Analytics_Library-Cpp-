#pragma once
#include "dal/ColumnBase.h"

#include <ostream>

namespace dal {

// Demonstrates abstraction and polymorphism for column visualization.
class Visualizer {
public:
    virtual ~Visualizer() = default;
    virtual void render(const ColumnBase& column, std::ostream& output) const = 0;
};

// Demonstrates inheritance by implementing a text histogram visualization strategy.
class HistogramViz final : public Visualizer {
public:
    void render(const ColumnBase& column, std::ostream& output) const override;
};

} // namespace dal
