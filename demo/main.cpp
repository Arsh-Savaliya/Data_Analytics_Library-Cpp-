#include "dal/CorrelationMatrixViz.h"
#include "dal/IO.h"
#include "dal/SummaryReport.h"
#include "dal/Visualizer.h"

#include <iostream>
#include <limits>
#include <memory>

using namespace std;

int main() {
    try {
        unique_ptr<dal::IImporter> importer = dal::IOFactory::makeImporter("data/students.csv");
        dal::DataSet students = importer->load("data/students.csv");

        cout << "Student data viewer\n";
        bool running = true;
        while (running) {
            cout << "\nChoose what to see:\n"
                 << "1. Show the table\n"
                 << "2. Show summary\n"
                 << "3. GPA histogram\n"
                 << "4. City bar chart\n"
                 << "5. GPA box summary\n"
                 << "6. Correlation matrix\n"
                 << "0. Exit\n"
                 << "Enter a number: ";

            int choice;
            if (!(cin >> choice)) {
                if (cin.eof()) break;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter a number from 0 to 6.\n";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            try {
                switch (choice) {
                case 1:
                    students.printTable();
                    break;
                case 2:
                    dal::SummaryReport(students).describe().printTable();
                    break;
                case 3:
                    dal::HistogramViz().render(students.getColumn("gpa"), cout);
                    break;
                case 4:
                    dal::BarChartViz().render(students.getColumn("city"), cout);
                    break;
                case 5:
                    dal::BoxSummaryViz().render(students.getColumn("gpa"), cout);
                    break;
                case 6:
                    dal::CorrelationMatrixViz().render(students, cout);
                    break;
                case 0:
                    running = false;
                    cout << "Goodbye.\n";
                    break;
                default:
                    cout << "Please choose a number from 0 to 6.\n";
                }
            } catch (const exception& error) {
                cout << "Could not show that: " << error.what() << '\n';
            }
        }
    } catch (const exception& error) {
        cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
