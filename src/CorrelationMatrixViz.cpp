#include "dal/CorrelationMatrixViz.h"
#include "dal/CorrelationAnalyzer.h"
#include <iomanip>
#include <stdexcept>
using namespace std;
namespace dal {
void CorrelationMatrixViz::render(const DataSet& data, ostream& output) const {
    vector<string> numeric;
    for (const auto& name : data.columnNames()) { if (data.getColumn(name).isNumeric()) { numeric.push_back(name); } }
    output << "Correlation matrix (+: positive, -: negative, .: near zero)\n" << setw(12) << "";
    for (const auto& name : numeric) { output << setw(12) << name; }
    output << '\n';
    CorrelationAnalyzer analyzer;
    for (const auto& left : numeric) {
        output << setw(12) << left;
        for (const auto& right : numeric) {
            try {
                const double value = analyzer.analyze(data.getColumn(left), data.getColumn(right));
                const char symbol = value > 0.25 ? '+' : (value < -0.25 ? '-' : '.');
                output << setw(12) << string(1U, symbol) + " " + to_string(value).substr(0U, 4U);
            } catch (const exception&) { output << setw(12) << "NA"; }
        }
        output << '\n';
    }
}
}
