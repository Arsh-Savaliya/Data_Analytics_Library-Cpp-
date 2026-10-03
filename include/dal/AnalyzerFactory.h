#pragma once
#include "dal/IAnalyzer.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
using namespace std;

namespace dal {
// Factory registry applies Open/Closed: extensions register creators without modifying existing analyzer code.
class AnalyzerFactory {
public:
    using Creator = function<unique_ptr<IAnalyzer>()>;
    AnalyzerFactory();
    void registerAnalyzer(const string& name, Creator creator);
    unique_ptr<IAnalyzer> create(const string& name) const;
private:
    map<string, Creator> registry_;
};
}
