#pragma once
#include "dal/DataSet.h"
#include <ostream>
using namespace std;
namespace dal {
// Prints a compact ASCII correlation matrix; matrix input needs more than one column.
class CorrelationMatrixViz {
public:
    void render(const DataSet& data, ostream& output) const;
};
}
