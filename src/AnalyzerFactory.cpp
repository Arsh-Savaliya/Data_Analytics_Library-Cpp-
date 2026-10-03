#include "dal/AnalyzerFactory.h"

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

#include <stdexcept>
#include <utility>
using namespace std;

namespace dal {

AnalyzerFactory::AnalyzerFactory() {
    // Each lambda is a small recipe for making one analyzer when it is requested.
    registerAnalyzer("mean", [] {
        return make_unique<MeanAnalyzer>();
    });
    registerAnalyzer("median", [] {
        return make_unique<MedianAnalyzer>();
    });
    registerAnalyzer("mode", [] {
        return make_unique<ModeAnalyzer>();
    });
    registerAnalyzer("stddev", [] {
        return make_unique<StdDevAnalyzer>();
    });
    registerAnalyzer("variance", [] {
        return make_unique<VarianceAnalyzer>();
    });
    registerAnalyzer("min", [] {
        return make_unique<MinAnalyzer>();
    });
    registerAnalyzer("max", [] {
        return make_unique<MaxAnalyzer>();
    });
    registerAnalyzer("sum", [] {
        return make_unique<SumAnalyzer>();
    });
    registerAnalyzer("count", [] {
        return make_unique<CountAnalyzer>();
    });
    registerAnalyzer("percentile", [] {
        return make_unique<PercentileAnalyzer>(50.0);
    });
}

void AnalyzerFactory::registerAnalyzer(const string& name, Creator creator) {
    if (name.empty()) {
        throw invalid_argument("Analyzer name cannot be empty");
    }
    if (!creator) {
        throw invalid_argument("Analyzer creator cannot be empty");
    }

    registry_[name] = move(creator);
}

unique_ptr<IAnalyzer> AnalyzerFactory::create(const string& name) const {
    const auto analyzer = registry_.find(name);
    if (analyzer == registry_.end()) {
        throw invalid_argument("Unknown analyzer: " + name);
    }

    return analyzer->second();
}

} // namespace dal
