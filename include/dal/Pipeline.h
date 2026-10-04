#pragma once

#include "dal/AnalyzerFactory.h"
#include "dal/DataSet.h"
#include "dal/Filters.h"
#include "dal/IO.h"
#include "dal/Visualizer.h"

#include <memory>
#include <ostream>
#include <string>
#include <vector>
using namespace std;

namespace dal {

// Every pipeline step shares one interface, so Pipeline can run different step types in order.
class IStep {
public:
    virtual ~IStep() = default;
    virtual void run(DataSet& data, ostream& output) const = 0;
};

// A step that filters rows and replaces the current dataset with the result.
class FilterStep final : public IStep {
public:
    explicit FilterStep(unique_ptr<IFilter> filter);
    void run(DataSet& data, ostream& output) const override;
private:
    unique_ptr<IFilter> filter_;
};

// A step that keeps the requested columns.
class SelectStep final : public IStep {
public:
    explicit SelectStep(vector<string> columnNames);
    void run(DataSet& data, ostream& output) const override;
private:
    vector<string> columnNames_;
};

// A step that applies each named analyzer to each numeric column.
class AnalyzeStep final : public IStep {
public:
    explicit AnalyzeStep(vector<string> analyzerNames);
    void run(DataSet& data, ostream& output) const override;
private:
    vector<string> analyzerNames_;
};

// A step that renders one selected column through any Visualizer implementation.
class VisualizeStep final : public IStep {
public:
    VisualizeStep(string heading, string columnName, unique_ptr<Visualizer> visualizer);
    void run(DataSet& data, ostream& output) const override;
private:
    string heading_;
    string columnName_;
    unique_ptr<Visualizer> visualizer_;
};

// A step that writes the current dataset through any IExporter implementation.
class ExportStep final : public IStep {
public:
    ExportStep(string path, unique_ptr<IExporter> exporter);
    void run(DataSet& data, ostream& output) const override;
private:
    string path_;
    unique_ptr<IExporter> exporter_;
};

// Pipeline owns its steps and passes one working dataset through them in insertion order.
class Pipeline {
public:
    void addStep(unique_ptr<IStep> step);
    DataSet run(const DataSet& input, ostream& output) const;
private:
    vector<unique_ptr<IStep>> steps_;
};

} // namespace dal
