#pragma once

#include "dal/Column.h"
#include "dal/DataSet.h"

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

namespace dal {

struct ImportedCell {
    optional<string> text;
    bool preserveString = false;
};

// Builds a DataSet one column at a time after checking the values' types.
class ImportTableBuilder {
public:
    static DataSet build(
        const vector<string>& headers,
        const vector<vector<ImportedCell>>& rows) {
        DataSet result;

        for (size_t columnIndex = 0; columnIndex < headers.size(); ++columnIndex) {
            bool containsQuotedString = false;
            bool everyValueIsInteger = true;
            bool everyValueIsNumber = true;

            // First pass decides which type can represent the whole column.
            for (const auto& row : rows) {
                const ImportedCell& cell = row[columnIndex];
                if (!cell.text.has_value()) {
                    continue;
                }

                if (cell.preserveString) {
                    containsQuotedString = true;
                }

                int integerValue = 0;
                if (!tryReadInteger(*cell.text, integerValue)) {
                    everyValueIsInteger = false;
                }

                double numberValue = 0.0;
                if (!tryReadNumber(*cell.text, numberValue)) {
                    everyValueIsNumber = false;
                }
            }

            if (containsQuotedString || !everyValueIsNumber) {
                result.addColumn(makeStringColumn(headers[columnIndex], rows, columnIndex));
            } else if (everyValueIsInteger) {
                result.addColumn(makeIntegerColumn(headers[columnIndex], rows, columnIndex));
            } else {
                result.addColumn(makeNumberColumn(headers[columnIndex], rows, columnIndex));
            }
        }

        return result;
    }

private:
    static bool tryReadInteger(const string& text, int& value) {
        istringstream input(text);
        input >> value;
        return input && input.eof();
    }

    static bool tryReadNumber(const string& text, double& value) {
        istringstream input(text);
        input >> value;
        return input && input.eof();
    }

    static unique_ptr<ColumnBase> makeIntegerColumn(
        const string& name,
        const vector<vector<ImportedCell>>& rows,
        size_t columnIndex) {
        auto column = make_unique<Column<int>>(name);
        for (const auto& row : rows) {
            if (!row[columnIndex].text.has_value()) {
                column->addMissing();
            } else {
                int value = 0;
                tryReadInteger(*row[columnIndex].text, value);
                column->addValue(value);
            }
        }
        return column;
    }

    static unique_ptr<ColumnBase> makeNumberColumn(
        const string& name,
        const vector<vector<ImportedCell>>& rows,
        size_t columnIndex) {
        auto column = make_unique<Column<double>>(name);
        for (const auto& row : rows) {
            if (!row[columnIndex].text.has_value()) {
                column->addMissing();
            } else {
                double value = 0.0;
                tryReadNumber(*row[columnIndex].text, value);
                column->addValue(value);
            }
        }
        return column;
    }

    static unique_ptr<ColumnBase> makeStringColumn(
        const string& name,
        const vector<vector<ImportedCell>>& rows,
        size_t columnIndex) {
        auto column = make_unique<Column<string>>(name);
        for (const auto& row : rows) {
            if (!row[columnIndex].text.has_value()) {
                column->addMissing();
            } else {
                column->addValue(*row[columnIndex].text);
            }
        }
        return column;
    }
};

} // namespace dal
