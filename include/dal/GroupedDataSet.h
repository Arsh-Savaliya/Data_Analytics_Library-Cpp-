#pragma once

#include "dal/DataSet.h"

#include <string>
using namespace std;

namespace dal {

// Holds a dataset snapshot and the column used to form groups.
class GroupedDataSet {
public:
    GroupedDataSet(const DataSet& source, string groupColumn);
    DataSet agg(const string& analyzerName, const string& valueColumn) const;

private:
    DataSet source_;
    string groupColumn_;
};

} // namespace dal
