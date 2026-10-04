#include "dal/SummaryReport.h"
#include "dal/AnalyzerFactory.h"
#include "dal/PercentileAnalyzer.h"
#include <map>
#include <iomanip>
#include <sstream>
using namespace std;
namespace dal {
SummaryReport::SummaryReport(const DataSet& data) : data_(data) {}
DataSet SummaryReport::describe() const {
    Column<string> statistic("statistic");
    const vector<string> rows{"count", "mean", "std", "min", "25%", "50%", "75%", "max", "unique", "top"};
    for (const auto& row : rows) { statistic.addValue(row); }
    DataSet result;
    result.addColumn(make_unique<Column<string>>(move(statistic)));
    AnalyzerFactory factory;
    for (const auto& name : data_.columnNames()) {
        const auto& column = data_.getColumn(name);
        Column<string> summary(name);
        for (int i = 0; i < 10; ++i) { summary.addMissing(); }
        if (column.isNumeric()) {
            const vector<string> analyzers{"count", "mean", "stddev", "min", "percentile", "percentile", "percentile", "max"};
            for (size_t i = 0; i < analyzers.size(); ++i) {
                try {
                    unique_ptr<IAnalyzer> analyzer;
                    if (i == 4U) { analyzer = make_unique<PercentileAnalyzer>(25.0); }
                    else if (i == 5U) { analyzer = make_unique<PercentileAnalyzer>(50.0); }
                    else if (i == 6U) { analyzer = make_unique<PercentileAnalyzer>(75.0); }
                    else { analyzer = factory.create(analyzers[i]); }
                    ostringstream formatted;
                    formatted << setprecision(6) << analyzer->analyze(column);
                    summary.set(i, formatted.str());
                } catch (const EmptyColumn&) { }
                catch (const domain_error&) { }
            }
        } else {
            map<string, size_t> counts;
            size_t count = 0U;
            for (size_t i = 0; i < column.size(); ++i) {
                if (!column.isMissing(i)) { ++counts[column.valueAsString(i)]; ++count; }
            }
            summary.set(0U, to_string(count));
            summary.set(8U, to_string(counts.size()));
            if (!counts.empty()) {
                auto top = counts.begin();
                for (auto it = counts.begin(); it != counts.end(); ++it) { if (it->second > top->second) { top = it; } }
                summary.set(9U, top->first);
            }
        }
        result.addColumn(make_unique<Column<string>>(move(summary)));
    }
    return result;
}
}
