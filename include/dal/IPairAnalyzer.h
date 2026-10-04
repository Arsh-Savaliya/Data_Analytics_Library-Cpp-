#pragma once
#include "dal/ColumnBase.h"
#include <string>
using namespace std;

namespace dal {
// Separate interface follows Interface Segregation: paired statistics require two inputs and should not burden IAnalyzer clients.
class IPairAnalyzer {
public:
    virtual ~IPairAnalyzer() = default;
    virtual double analyze(const ColumnBase& first, const ColumnBase& second) const = 0;
    virtual string name() const = 0;
};
}
