#include "dal/Column.h"
#include "dal/Filters.h"
#include "dal/IO.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <cstdio>
using namespace std;
int main() {
    const string path = "generated_100k.csv";
    {
        ofstream file(path);
        file << "id,age,city\n";
        for (int i = 0; i < 100000; ++i) { file << i << ',' << (18 + i % 50) << ",City" << (i % 20) << '\n'; }
    }
    const auto loadStart = chrono::steady_clock::now();
    dal::DataSet data = dal::CsvImporter().load(path);
    const auto loadEnd = chrono::steady_clock::now();
    const auto filterStart = chrono::steady_clock::now();
    const auto filtered = data.filterBy(dal::gt("age", 40));
    const auto filterEnd = chrono::steady_clock::now();
    cout << "100,000 rows: load " << chrono::duration_cast<chrono::milliseconds>(loadEnd - loadStart).count()
         << " ms; filter " << chrono::duration_cast<chrono::milliseconds>(filterEnd - filterStart).count()
         << " ms; matched " << filtered.rowCount() << '\n';
    std::remove(path.c_str());
    return 0;
}
