#include "dal/Column.h"
#include "dal/DataSet.h"
#include "dal/MeanAnalyzer.h"

#include <iostream>
#include <memory>
using namespace std;

int main() {
    dal::DataSet students;
    auto scores = make_unique<dal::Column<double>>("score");
    scores->addValue(82.0);
    scores->addValue(91.0);
    scores->addValue(87.0);
    students.addColumn(move(scores));

    const auto analyzer = make_unique<dal::MeanAnalyzer>();
    const double result = analyzer->analyze(students.getColumn("score"));
    cout << analyzer->name() << " score: " << result << '\n';
    return 0;
}
