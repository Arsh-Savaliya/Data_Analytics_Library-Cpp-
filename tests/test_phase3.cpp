#include "dal/AnalyzerDecorator.h"
#include "dal/CommandShell.h"
#include "dal/CorrelationMatrixViz.h"
#include "dal/GroupedDataSet.h"
#include "dal/MeanAnalyzer.h"
#include "dal/SummaryReport.h"
#include "dal/Column.h"
#include "dal/Filters.h"
#include <cassert>
#include <memory>
#include <sstream>
using namespace std;
int main() {
    dal::DataSet data;
    auto groups = make_unique<dal::Column<string>>("group", vector<optional<string>>{"A", "A", "B", nullopt});
    auto values = make_unique<dal::Column<double>>("value", vector<optional<double>>{1.0, 3.0, 5.0, 7.0});
    auto ids = make_unique<dal::Column<int>>("id", vector<optional<int>>{1, 2, 3, 4});
    data.addColumn(move(groups)); data.addColumn(move(values)); data.addColumn(move(ids));

    dal::DataSet moved(move(data));
    assert(moved.rowCount() == 4U && data.rowCount() == 0U);
    dal::DataSet moveAssigned;
    moveAssigned = move(moved);
    assert(moveAssigned.rowCount() == 4U && moved.rowCount() == 0U);
    const auto sorted = moveAssigned.sortBy("value", false);
    assert(sorted.getColumnAs<double>("value").at(0).value() == 7.0);
    assert(moveAssigned.sortBy("value").getColumnAs<double>("value").at(0).value() == 1.0);
    const auto grouped = moveAssigned.groupBy("group").agg("mean", "value");
    assert(grouped.rowCount() == 3U);
    assert(grouped.getColumnAs<double>("mean_value").at(1).value() == 2.0);

    dal::DataSet right;
    right.addColumn(make_unique<dal::Column<int>>("id", vector<optional<int>>{2, 3, 9}));
    right.addColumn(make_unique<dal::Column<string>>("tag", vector<optional<string>>{"x", "y", "z"}));
    const auto joined = moveAssigned.join(right, "id");
    assert(joined.rowCount() == 2U);
    assert(joined.getColumnAs<string>("tag").at(0).value() == "x");

    dal::SummaryReport report(moveAssigned);
    const auto description = report.describe();
    assert(description.getColumnAs<string>("value").at(0).value() == "4");
    assert(description.getColumnAs<string>("group").at(8).value() == "2");

    dal::RoundingAnalyzer rounded(make_unique<dal::MeanAnalyzer>(), 1);
    assert(rounded.analyze(moveAssigned.getColumn("value")) == 4.0);
    ostringstream matrix;
    dal::CorrelationMatrixViz().render(moveAssigned, matrix);
    assert(matrix.str().find("value") != string::npos);

    dal::CommandShell shell;
    ostringstream shellOutput;
    shell.execute("load data/students.csv", shellOutput);
    const size_t originalRows = shell.data().rowCount();
    shell.execute("filter age>18", shellOutput);
    assert(shell.data().rowCount() < originalRows);
    shell.execute("select name,gpa", shellOutput);
    assert(shell.data().columnCount() == 2U);
    shell.execute("describe", shellOutput);
    shell.execute("hist gpa", shellOutput);
    bool invalidCommand = false;
    try { shell.execute("unknown", shellOutput); } catch (const invalid_argument&) { invalidCommand = true; }
    assert(invalidCommand);
    return 0;
}
