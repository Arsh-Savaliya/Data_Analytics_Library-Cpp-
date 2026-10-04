#pragma once
#include "dal/DataSet.h"
using namespace std;
namespace dal {
// Owns a snapshot so reports stay valid if the caller later changes its DataSet.
class SummaryReport {
public:
    explicit SummaryReport(const DataSet& data);
    DataSet describe() const;
private:
    DataSet data_;
};
}
