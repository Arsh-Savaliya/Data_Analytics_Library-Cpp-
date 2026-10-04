#include "dal/GroupedDataSet.h"
#include "dal/AnalyzerFactory.h"
#include "dal/exceptions.h"

#include <map>
using namespace std;

namespace dal {

GroupedDataSet::GroupedDataSet(const DataSet& source, string groupColumn)
    : source_(source), groupColumn_(move(groupColumn)) {
    (void)source_.getColumn(groupColumn_);
}

DataSet GroupedDataSet::agg(const string& analyzerName, const string& valueColumn) const {
    struct Group {
        string label;
        vector<size_t> rows;
    };
    map<string, Group> groups;
    const auto& grouping = source_.getColumn(groupColumn_);
    for (size_t row = 0; row < source_.rowCount(); ++row) {
        const string key = grouping.isMissing(row) ? string("M:") : string("V:") + grouping.keyAt(row);
        auto& group = groups[key];
        if (group.rows.empty()) {
            group.label = grouping.isMissing(row) ? "NA" : grouping.valueAsString(row);
        }
        group.rows.push_back(row);
    }

    auto analyzer = AnalyzerFactory().create(analyzerName);
    const auto& values = source_.getColumn(valueColumn);
    Column<string> labels(groupColumn_);
    Column<double> aggregates(analyzerName + "_" + valueColumn);
    for (const auto& entry : groups) {
        labels.addValue(entry.second.label);
        auto subset = values.copyRows(entry.second.rows);
        try {
            aggregates.addValue(analyzer->analyze(*subset));
        } catch (const EmptyColumn&) {
            aggregates.addMissing();
        }
    }
    DataSet result;
    result.addColumn(make_unique<Column<string>>(move(labels)));
    result.addColumn(make_unique<Column<double>>(move(aggregates)));
    return result;
}

} // namespace dal
