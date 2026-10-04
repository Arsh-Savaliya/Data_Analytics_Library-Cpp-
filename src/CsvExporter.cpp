#include "dal/IO.h"
#include "dal/exceptions.h"

#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
using namespace std;

namespace dal {

void CsvExporter::writeField(ostream& output, const string& value) const {
    const bool needsQuotes = value.find_first_of(",\"\r\n") != string::npos;
    if (!needsQuotes) {
        output << value;
        return;
    }

    output << '"';
    for (char character : value) {
        if (character == '"') {
            output << "\"\"";
        } else {
            output << character;
        }
    }
    output << '"';
}

void CsvExporter::save(const DataSet& data, const string& path) {
    ofstream output(path, ios::binary);
    if (!output) {
        throw FileError("Could not open CSV output: " + path);
    }

    const vector<string> names = data.columnNames();
    for (size_t columnIndex = 0; columnIndex < names.size(); ++columnIndex) {
        if (columnIndex != 0U) {
            output << ',';
        }
        writeField(output, names[columnIndex]);
    }
    output << '\n';

    for (size_t row = 0; row < data.rowCount(); ++row) {
        for (size_t columnIndex = 0; columnIndex < names.size(); ++columnIndex) {
            if (columnIndex != 0U) {
                output << ',';
            }

            const ColumnBase& column = data.getColumn(names[columnIndex]);
            if (!column.isMissing(row)) {
                if (column.typeName() == "double") {
                    ostringstream number;
                    number << setprecision(numeric_limits<double>::max_digits10)
                           << column.toDouble(row);
                    writeField(output, number.str());
                } else {
                    writeField(output, column.valueAsString(row));
                }
            }
        }
        output << '\n';
    }

    if (!output) {
        throw FileError("Failed while writing CSV output: " + path);
    }
}

} // namespace dal
