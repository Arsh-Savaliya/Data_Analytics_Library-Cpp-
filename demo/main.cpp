
#include "dal/AnalyzerFactory.h"
#include "dal/CommandShell.h"
#include "dal/CorrelationMatrixViz.h"
#include "dal/Filters.h"
#include "dal/GroupedDataSet.h"
#include "dal/IO.h"
#include "dal/Pipeline.h"
#include "dal/SummaryReport.h"
#include "dal/Visualizer.h"

#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace {

void showMenu() {
    cout << "\n========== DATA ANALYTICS LIBRARY ==========\n"
         << " 1. Display dataset\n"
         << " 2. Load CSV/JSON file\n"
         << " 3. Summary report\n"
         << " 4. Run a statistical analyzer\n"
         << " 5. Histogram\n"
         << " 6. Bar chart\n"
         << " 7. Box summary\n"
         << " 8. Correlation matrix\n"
         << " 9. Filter numeric column\n"
         << "10. Select columns\n"
         << "11. Sort dataset\n"
         << "12. Show first N rows\n"
         << "13. Show last N rows\n"
         << "14. Group and aggregate\n"
         << "15. Export CSV/JSON\n"
         << "16. Run a processing pipeline\n"
         << "17. Advanced command shell\n"
         << " 0. Exit\n"
         << "Choose an option: ";
}

int readInt(const string& prompt) {
    int value;
    cout << prompt;

    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a valid integer: ";
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

string readLine(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

vector<string> splitNames(const string& input) {
    vector<string> names;
    string part;
    istringstream stream(input);

    while (getline(stream, part, ',')) {
        const auto first = part.find_first_not_of(" \t");
        if (first == string::npos) {
            continue;
        }

        const auto last = part.find_last_not_of(" \t");
        names.push_back(part.substr(first, last - first + 1));
    }

    return names;
}

void printColumns(const dal::DataSet& data) {
    cout << "Available columns: ";
    const auto names = data.columnNames();

    for (size_t i = 0; i < names.size(); ++i) {
        if (i != 0) cout << ", ";
        cout << names[i];
    }
    cout << '\n';
}

void runShell(dal::DataSet& data) {
    dal::CommandShell shell(data);

    cout << "\nAdvanced command shell\n"
         << "Commands: load <file>, show, filter <column><op><value>,\n"
         << "          select <col1,col2>, describe, hist <column>, export <file>\n"
         << "Type quit to return to the main menu.\n"
         << "The shell starts with the current dataset.\n";

    string command;
    while (true) {
        cout << "dal> ";
        if (!getline(cin, command)) {
            cin.clear();
            break;
        }

        if (command == "quit" || command == "exit") {
            break;
        }

        try {
            shell.execute(command, cout);
        } catch (const exception& error) {
            cout << "Command error: " << error.what() << '\n';
        }
    }
    data = shell.data();
}

void runPipeline(const dal::DataSet& data) {
    cout << "\nPipeline demonstration:\n"
         << "This example summarizes numeric columns and leaves the original dataset unchanged.\n";

    dal::Pipeline pipeline;
    pipeline.addStep(make_unique<dal::AnalyzeStep>(
        vector<string>{"mean", "min", "max"}));

    try {
        dal::DataSet result = pipeline.run(data, cout);
        (void)result;
        cout << "Pipeline completed.\n";
    } catch (const exception& error) {
        cout << "Pipeline error: " << error.what() << '\n';
    }
}

} // namespace

int main() {
    try {
        // Load the example dataset at startup.
        unique_ptr<dal::IImporter> importer =
            dal::IOFactory::makeImporter("data/students.csv");

        dal::DataSet data = importer->load("data/students.csv");

        cout << "Loaded data/students.csv successfully.\n";
        cout << "Rows: " << data.rowCount()
             << " | Columns: " << data.columnCount() << '\n';

        bool running = true;

        while (running) {
            showMenu();

            int choice;
            try {
                choice = readInt("");
            } catch (const exception&) {
                break;
            }

            try {
                switch (choice) {
                case 1:
                    data.printTable();
                    break;

                case 2: {
                    const string path = readLine(
                        "File path (.csv or .json): ");

                    data = dal::IOFactory::makeImporter(path)->load(path);

                    cout << "Loaded successfully. Rows: "
                         << data.rowCount() << '\n';
                    printColumns(data);
                    break;
                }

                case 3:
                    dal::SummaryReport(data).describe().printTable();
                    break;

                case 4: {
                    printColumns(data);
                    const string column = readLine("Column name: ");
                    const string analyzerName = readLine(
                        "Analyzer (mean, median, mode, min, max, sum, "
                        "count, variance, stddev, percentile): ");

                    dal::AnalyzerFactory factory;
                    auto analyzer = factory.create(analyzerName);

                    const double result =
                        analyzer->analyze(data.getColumn(column));

                    cout << analyzer->name() << "(" << column
                         << ") = " << result << '\n';
                    break;
                }

                case 5: {
                    printColumns(data);
                    const string column = readLine("Column name: ");
                    dal::HistogramViz().render(data.getColumn(column), cout);
                    break;
                }

                case 6: {
                    printColumns(data);
                    const string column = readLine("Column name: ");
                    dal::BarChartViz().render(data.getColumn(column), cout);
                    break;
                }

                case 7: {
                    printColumns(data);
                    const string column = readLine("Numeric column: ");
                    dal::BoxSummaryViz().render(data.getColumn(column), cout);
                    break;
                }

                case 8:
                    dal::CorrelationMatrixViz().render(data, cout);
                    break;

                case 9: {
                    printColumns(data);
                    const string column = readLine("Numeric column: ");
                    const double threshold = stod(
                        readLine("Keep rows where value is greater than: "));

                    auto filter = make_unique<dal::ComparisonFilter<double>>(
                        column,
                        dal::ComparisonFilter<double>::Operator::GT,
                        threshold);

                    data = data.filterBy(*filter);
                    cout << "Filter applied. Remaining rows: "
                         << data.rowCount() << '\n';
                    break;
                }

                case 10: {
                    printColumns(data);
                    const auto names = splitNames(
                        readLine("Columns to keep (comma-separated): "));

                    data = data.select(names);
                    cout << "Selected columns. Current dataset:\n";
                    data.printTable();
                    break;
                }

                case 11: {
                    printColumns(data);
                    const string column = readLine("Sort by column: ");
                    const string order = readLine("Order (asc/desc): ");

                    data = data.sortBy(column, order != "desc");
                    data.printTable();
                    break;
                }

                case 12: {
                    const int n = readInt("Number of rows: ");
                    if (n < 0) throw invalid_argument("N cannot be negative.");
                    data.head(static_cast<size_t>(n)).printTable();
                    break;
                }

                case 13: {
                    const int n = readInt("Number of rows: ");
                    if (n < 0) throw invalid_argument("N cannot be negative.");
                    data.tail(static_cast<size_t>(n)).printTable();
                    break;
                }

                case 14: {
                    printColumns(data);
                    const string groupColumn = readLine("Group by column: ");
                    const string valueColumn = readLine("Value column: ");
                    const string analyzerName = readLine(
                        "Aggregation (mean, sum, count, min, max): ");

                    data.groupBy(groupColumn)
                        .agg(analyzerName, valueColumn)
                        .printTable();
                    break;
                }

                case 15: {
                    const string path = readLine(
                        "Output path (.csv or .json): ");

                    dal::IOFactory::makeExporter(path)->save(data, path);
                    cout << "Exported successfully to " << path << '\n';
                    break;
                }

                case 16:
                    runPipeline(data);
                    break;

                case 17:
                    runShell(data);
                    break;

                case 0:
                    running = false;
                    cout << "Goodbye!\n";
                    break;

                default:
                    cout << "Choose an option from 0 to 17.\n";
                }
            } catch (const exception& error) {
                cout << "Operation failed: " << error.what() << '\n';
            }
        }
    } catch (const exception& error) {
        cerr << "Startup error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
