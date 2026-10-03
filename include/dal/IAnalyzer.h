#pragma once
#include "dal/ColumnBase.h"

#include <string>
using namespace std;

namespace dal {

// Strategy interface: callers choose an algorithm through this contract without owning its data.
class IAnalyzer {
public:
    virtual ~IAnalyzer() = default;
    virtual double analyze(const ColumnBase& column) const = 0;
    virtual string name() const = 0;
};

} // namespace dal
