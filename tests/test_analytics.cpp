#include "dal/AnalyzerFactory.h"
#include "dal/Column.h"
#include "dal/CorrelationAnalyzer.h"
#include "dal/CovarianceAnalyzer.h"
#include "dal/CountAnalyzer.h"
#include "dal/MaxAnalyzer.h"
#include "dal/MeanAnalyzer.h"
#include "dal/MedianAnalyzer.h"
#include "dal/MinAnalyzer.h"
#include "dal/ModeAnalyzer.h"
#include "dal/PercentileAnalyzer.h"
#include "dal/StdDevAnalyzer.h"
#include "dal/SumAnalyzer.h"
#include "dal/VarianceAnalyzer.h"
#include "dal/Visualizer.h"

#include <cassert>
#include <cmath>
#include <memory>
#include <sstream>
using namespace std;

namespace {
bool near(double left, double right) { return abs(left - right) < 1e-9; }
class RangeAnalyzer final : public dal::IAnalyzer {
public:
    double analyze(const dal::ColumnBase& column) const override { return column.toDouble(column.size() - 1U) - column.toDouble(0U); }
    string name() const override { return "range"; }
};
}

int main() {
    dal::Column<double> odd("odd", {1.0, 3.0, 2.0, nullopt, 4.0});
    assert(near(dal::MeanAnalyzer{}.analyze(odd), 2.5));
    assert(near(dal::MedianAnalyzer{}.analyze(odd), 2.5));
    assert(near(dal::MinAnalyzer{}.analyze(odd), 1.0));
    assert(near(dal::MaxAnalyzer{}.analyze(odd), 4.0));
    assert(near(dal::SumAnalyzer{}.analyze(odd), 10.0));
    assert(near(dal::CountAnalyzer{}.analyze(odd), 4.0));
    assert(near(dal::PercentileAnalyzer(25.0).analyze(odd), 1.75));

    dal::Column<int> even("even", {1, 2, 3, 4});
    assert(near(dal::MedianAnalyzer{}.analyze(even), 2.5));
    dal::Column<int> oddLength("odd-length", {1, 9, 3});
    assert(near(dal::MedianAnalyzer{}.analyze(oddLength), 3.0));
    dal::Column<int> tied("tied", {5, 1, 5, 1, 3});
    assert(near(dal::ModeAnalyzer{}.analyze(tied), 1.0));

    dal::Column<double> known("known", {2.0, 4.0, 6.0});
    assert(near(dal::VarianceAnalyzer(false).analyze(known), 8.0 / 3.0));
    assert(near(dal::StdDevAnalyzer(false).analyze(known), sqrt(8.0 / 3.0)));
    assert(near(dal::VarianceAnalyzer(true).analyze(known), 4.0));
    assert(near(dal::StdDevAnalyzer(true).analyze(known), 2.0));

    dal::Column<double> x("x", {1.0, 2.0, 3.0, nullopt});
    dal::Column<double> y("y", {2.0, 4.0, 6.0, 99.0});
    assert(near(dal::CorrelationAnalyzer{}.analyze(x, y), 1.0));
    assert(near(dal::CovarianceAnalyzer{}.analyze(x, y), 2.0));

    dal::AnalyzerFactory factory;
    assert(near(factory.create("mean")->analyze(known), 4.0));
    assert(factory.create("stddev")->name() == "stddev");
    factory.registerAnalyzer("range", [] { return make_unique<RangeAnalyzer>(); });
    assert(near(factory.create("range")->analyze(known), 4.0));

    ostringstream histogram;
    dal::HistogramViz(2U).render(known, histogram);
    assert(histogram.str().find('#') != string::npos);
    dal::Column<string> categories("category", {"A", "B", "A", nullopt});
    ostringstream bars;
    dal::BarChartViz{}.render(categories, bars);
    assert(bars.str().find("A | ## 2") != string::npos);
    ostringstream box;
    dal::BoxSummaryViz{}.render(known, box);
    assert(box.str().find("2 |--[") != string::npos);
    return 0;
}
