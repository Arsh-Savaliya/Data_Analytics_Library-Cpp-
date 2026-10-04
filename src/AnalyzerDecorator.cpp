#include "dal/AnalyzerDecorator.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>
using namespace std;
namespace dal {
AnalyzerDecorator::AnalyzerDecorator(unique_ptr<IAnalyzer> wrapped) : wrapped_(move(wrapped)) {
    if (!wrapped_) { throw invalid_argument("Analyzer decorator needs an analyzer"); }
}
string AnalyzerDecorator::name() const { return wrapped_->name(); }
LoggingAnalyzer::LoggingAnalyzer(unique_ptr<IAnalyzer> wrapped) : AnalyzerDecorator(move(wrapped)) {}
double LoggingAnalyzer::analyze(const ColumnBase& column) const {
    const auto start = chrono::steady_clock::now();
    const double value = wrapped_->analyze(column);
    const auto elapsed = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start).count();
    cout << "[timing] " << name() << " took " << elapsed << " us\n";
    return value;
}
RoundingAnalyzer::RoundingAnalyzer(unique_ptr<IAnalyzer> wrapped, int decimals)
    : AnalyzerDecorator(move(wrapped)), decimals_(decimals) {
    if (decimals_ < 0 || decimals_ > 12) { throw invalid_argument("Rounding precision must be from 0 to 12"); }
}
double RoundingAnalyzer::analyze(const ColumnBase& column) const {
    const double scale = pow(10.0, decimals_);
    return round(wrapped_->analyze(column) * scale) / scale;
}
}
