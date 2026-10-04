#include "dal/CommandShell.h"
#include "dal/AnalyzerFactory.h"
#include "dal/Filters.h"
#include "dal/IO.h"
#include "dal/SummaryReport.h"
#include "dal/Visualizer.h"
#include <sstream>
#include <stdexcept>
using namespace std;
namespace dal {
namespace {
class LoadCommand final : public ICommand {
public: explicit LoadCommand(string path) : path_(move(path)) {}
    void execute(DataSet& data, ostream& output) const override {
        data = IOFactory::makeImporter(path_)->load(path_); output << "Loaded " << data.rowCount() << " rows\n";
    }
private: string path_;
};
class ShowCommand final : public ICommand {
public: void execute(DataSet& data, ostream& output) const override { data.printTable(output); }
};
class SelectCommand final : public ICommand {
public: explicit SelectCommand(vector<string> names) : names_(move(names)) {}
    void execute(DataSet& data, ostream&) const override { data = data.select(names_); }
private: vector<string> names_;
};
class FilterCommand final : public ICommand {
public:
    explicit FilterCommand(string expression) : expression_(move(expression)) {}
    void execute(DataSet& data, ostream&) const override {
        const vector<string> ops{">=", "<=", "!=", "==", ">", "<", "="};
        size_t position = string::npos; string op;
        for (const auto& candidate : ops) { position = expression_.find(candidate); if (position != string::npos) { op = candidate; break; } }
        if (position == string::npos) { throw invalid_argument("Expected filter such as age>18"); }
        const string name = expression_.substr(0U, position);
        const string literal = expression_.substr(position + op.size());
        if (name.empty() || literal.empty()) { throw invalid_argument("Filter needs a column and value"); }
        auto filter = makeFilter(data.getColumn(name), name, op, literal);
        data = data.filterBy(*filter);
    }
private:
    static unique_ptr<IFilter> makeFilter(const ColumnBase& column, const string& name, const string& op, const string& literal) {
        const auto parseOp = [&]() {
            using O = ComparisonFilter<string>::Operator;
            if (op == "==" || op == "=") { return O::EQ; }
            if (op == "!=") { return O::NE; }
            if (op == "<") { return O::LT; }
            if (op == "<=") { return O::LE; }
            if (op == ">") { return O::GT; }
            return O::GE;
        };
        if (column.typeName() == "int") return make_unique<ComparisonFilter<int>>(name, static_cast<ComparisonFilter<int>::Operator>(parseOp()), stoi(literal));
        if (column.typeName() == "double") return make_unique<ComparisonFilter<double>>(name, static_cast<ComparisonFilter<double>::Operator>(parseOp()), stod(literal));
        return make_unique<ComparisonFilter<string>>(name, parseOp(), literal);
    }
    string expression_;
};
class DescribeCommand final : public ICommand {
public: void execute(DataSet& data, ostream& output) const override { SummaryReport(data).describe().printTable(output); }
};
class HistogramCommand final : public ICommand {
public: explicit HistogramCommand(string name) : name_(move(name)) {}
    void execute(DataSet& data, ostream& output) const override { HistogramViz().render(data.getColumn(name_), output); }
private: string name_;
};
class ExportCommand final : public ICommand {
public: explicit ExportCommand(string path) : path_(move(path)) {}
    void execute(DataSet& data, ostream& output) const override { IOFactory::makeExporter(path_)->save(data, path_); output << "Exported " << path_ << '\n'; }
private: string path_;
};
}
void CommandShell::execute(const string& line, ostream& output) {
    istringstream input(line); string command; input >> command;
    if (command.empty() || command[0] == '#') { return; }
    string argument; getline(input, argument);
    const auto first = argument.find_first_not_of(" \t");
    if (first != string::npos) argument.erase(0U, first); else argument.clear();
    unique_ptr<ICommand> parsed;
    if (command == "load" && !argument.empty()) parsed = make_unique<LoadCommand>(argument);
    else if (command == "show" && argument.empty()) parsed = make_unique<ShowCommand>();
    else if (command == "select" && !argument.empty()) {
        vector<string> names; string part; istringstream list(argument);
        while (getline(list, part, ',')) { names.push_back(part); }
        parsed = make_unique<SelectCommand>(move(names));
    } else if (command == "filter" && !argument.empty()) parsed = make_unique<FilterCommand>(argument);
    else if (command == "describe" && argument.empty()) parsed = make_unique<DescribeCommand>();
    else if (command == "hist" && !argument.empty()) parsed = make_unique<HistogramCommand>(argument);
    else if (command == "export" && !argument.empty()) parsed = make_unique<ExportCommand>(argument);
    else throw invalid_argument("Unknown or incomplete command: " + command);
    parsed->execute(data_, output);
}
void CommandShell::run(istream& input, ostream& output) {
    string line;
    while (getline(input, line)) { execute(line, output); }
}
const DataSet& CommandShell::data() const { return data_; }
}
