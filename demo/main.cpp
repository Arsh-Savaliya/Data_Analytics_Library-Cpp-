#include "dal/Filters.h"
#include "dal/IO.h"
#include "dal/Pipeline.h"
#include "dal/AnalyzerDecorator.h"
#include "dal/AnalyzerFactory.h"
#include "dal/CorrelationMatrixViz.h"
#include "dal/SummaryReport.h"

#include <iostream>
#include <memory>
#include <vector>
using namespace std;

int main() {
    try {
        cout << "=== 1. Load the student data ===\n";
        unique_ptr<dal::IImporter> importer = dal::IOFactory::makeImporter("data/students.csv");
        dal::DataSet students = importer->load("data/students.csv");

        cout << "\n=== 2. Original table ===\n";
        students.printTable();

        cout << "\n=== 3. Run the analysis pipeline ===\n";
        dal::FilterHandle condition = dal::gt("gpa", 8.0) && dal::lt("age", 22);

        dal::Pipeline pipeline;
        pipeline.addStep(make_unique<dal::FilterStep>(condition.release()));
        pipeline.addStep(make_unique<dal::SelectStep>(
            vector<string>{"id", "name", "age", "gpa", "city"}));
        pipeline.addStep(make_unique<dal::AnalyzeStep>(
            vector<string>{"mean", "median", "min", "max", "count"}));
        pipeline.addStep(make_unique<dal::VisualizeStep>(
            "GPA histogram", "gpa", make_unique<dal::HistogramViz>(5U)));
        pipeline.addStep(make_unique<dal::VisualizeStep>(
            "City category counts", "city", make_unique<dal::BarChartViz>()));
        pipeline.addStep(make_unique<dal::ExportStep>(
            "data/filtered_students.json", dal::IOFactory::makeExporter("data/filtered_students.json")));
        pipeline.addStep(make_unique<dal::ExportStep>(
            "data/filtered_students.csv", dal::IOFactory::makeExporter("data/filtered_students.csv")));

        const dal::DataSet filteredStudents = pipeline.run(students, cout);

        cout << "\n=== 4. Filtered table ===\n";
        filteredStudents.printTable();
        cout << "\n=== 5. Describe summary ===\n";
        dal::SummaryReport(students).describe().printTable(cout);
        cout << "\n=== 6. Decorator example (logged and rounded mean) ===\n";
        dal::AnalyzerFactory analyzerFactory;
        unique_ptr<dal::IAnalyzer> decorated = make_unique<dal::RoundingAnalyzer>(
            make_unique<dal::LoggingAnalyzer>(analyzerFactory.create("mean")), 2);
        cout << "Mean GPA: " << decorated->analyze(students.getColumn("gpa")) << '\n';
        cout << "\n=== 7. Correlation heat-map ===\n";
        dal::CorrelationMatrixViz().render(students, cout);
        cout << "\nPipeline finished.\n";
    } catch (const exception& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
