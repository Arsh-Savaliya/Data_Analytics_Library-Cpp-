#include "dal/Pipeline.h"

#include "dal/exceptions.h"

#include <stdexcept>
#include <utility>

namespace dal {

FilterStep::FilterStep(unique_ptr<IFilter> filter)
    : filter_(move(filter)) {
    if (!filter_) {
        throw invalid_argument("FilterStep requires a filter");
    }
}

void FilterStep::run(DataSet& data, ostream& output) const {
    const size_t oldRowCount = data.rowCount();
    data = data.filterBy(*filter_);
    output << "Step: filter rows (" << oldRowCount << " -> " << data.rowCount() << ")\n";
}

SelectStep::SelectStep(vector<string> columnNames)
    : columnNames_(move(columnNames)) {}

void SelectStep::run(DataSet& data, ostream& output) const {
    data = data.select(columnNames_);
    output << "Step: select columns";
    for (const string& name : columnNames_) {
        output << ' ' << name;
    }
    output << '\n';
}

AnalyzeStep::AnalyzeStep(vector<string> analyzerNames)
    : analyzerNames_(move(analyzerNames)) {}

void AnalyzeStep::run(DataSet& data, ostream& output) const {
    output << "Step: summary report for numeric columns\n";
    AnalyzerFactory factory;

    for (const string& columnName : data.columnNames()) {
        const ColumnBase& column = data.getColumn(columnName);
        if (!column.isNumeric()) {
            continue;
        }

        output << "  " << columnName << ":\n";
        for (const string& analyzerName : analyzerNames_) {
            unique_ptr<IAnalyzer> analyzer = factory.create(analyzerName);
            output << "    " << analyzer->name() << " = ";
            try {
                output << analyzer->analyze(column) << '\n';
            } catch (const EmptyColumn&) {
                output << "NA (no values)\n";
            }
        }
    }
}

VisualizeStep::VisualizeStep(
    string heading,
    string columnName,
    unique_ptr<Visualizer> visualizer)
    : heading_(move(heading)),
      columnName_(move(columnName)),
      visualizer_(move(visualizer)) {
    if (!visualizer_) {
        throw invalid_argument("VisualizeStep requires a visualizer");
    }
}

void VisualizeStep::run(DataSet& data, ostream& output) const {
    output << "Step: " << heading_ << '\n';
    visualizer_->render(data.getColumn(columnName_), output);
}

ExportStep::ExportStep(string path, unique_ptr<IExporter> exporter)
    : path_(move(path)), exporter_(move(exporter)) {
    if (!exporter_) {
        throw invalid_argument("ExportStep requires an exporter");
    }
}

void ExportStep::run(DataSet& data, ostream& output) const {
    exporter_->save(data, path_);
    output << "Step: exported " << path_ << '\n';
}

void Pipeline::addStep(unique_ptr<IStep> step) {
    if (!step) {
        throw invalid_argument("Pipeline cannot store a null step");
    }
    steps_.push_back(move(step));
}

DataSet Pipeline::run(const DataSet& input, ostream& output) const {
    // Work on a deep copy, so running a pipeline does not change the caller's dataset.
    DataSet workingData(input);
    for (const auto& step : steps_) {
        step->run(workingData, output);
    }
    return workingData;
}

} // namespace dal
