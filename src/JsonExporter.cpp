#include "dal/IO.h"
#include "dal/exceptions.h"

#include <fstream>
#include <iomanip>
#include <limits>
using namespace std;

namespace dal {

void JsonExporter::writeString(ostream& output, const string& value) const {
    output << '"';

    for (unsigned char character : value) {
        if (character == '"') {
            output << "\\\"";
        } else if (character == '\\') {
            output << "\\\\";
        } else if (character == '\b') {
            output << "\\b";
        } else if (character == '\f') {
            output << "\\f";
        } else if (character == '\n') {
            output << "\\n";
        } else if (character == '\r') {
            output << "\\r";
        } else if (character == '\t') {
            output << "\\t";
        } else if (character < 0x20U) {
            output << "\\u" << hex << setw(4) << setfill('0')
                   << static_cast<unsigned int>(character);
            output << dec << setfill(' ');
        } else {
            output << static_cast<char>(character);
        }
    }

    output << '"';
}

void JsonExporter::save(const DataSet& data, const string& path) {
    ofstream output(path, ios::binary);
    if (!output) {
        throw FileError("Could not open JSON output: " + path);
    }

    const vector<string> names = data.columnNames();
    output << '[';

    for (size_t row = 0; row < data.rowCount(); ++row) {
        if (row != 0U) {
            output << ',';
        }
        output << '{';

        for (size_t columnIndex = 0; columnIndex < names.size(); ++columnIndex) {
            if (columnIndex != 0U) {
                output << ',';
            }

            writeString(output, names[columnIndex]);
            output << ':';

            const ColumnBase& column = data.getColumn(names[columnIndex]);
            if (column.isMissing(row)) {
                output << "null";
            } else if (column.typeName() == "string") {
                writeString(output, column.valueAsString(row));
            } else if (column.typeName() == "double") {
                output << setprecision(numeric_limits<double>::max_digits10)
                       << column.toDouble(row);
            } else {
                output << column.valueAsString(row);
            }
        }

        output << '}';
    }

    output << "]\n";
    if (!output) {
        throw FileError("Failed while writing JSON output: " + path);
    }
}

} // namespace dal
