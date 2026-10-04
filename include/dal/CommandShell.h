#pragma once
#include "dal/DataSet.h"
#include <iosfwd>
#include <memory>
#include <string>
using namespace std;
namespace dal {
// Command pattern shell converts text into polymorphic command objects before execution.
class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute(DataSet& data, ostream& output) const = 0;
};
class CommandShell {
public:
    CommandShell() = default;
    void execute(const string& line, ostream& output);
    void run(istream& input, ostream& output);
    const DataSet& data() const;
private:
    DataSet data_;
};
}
