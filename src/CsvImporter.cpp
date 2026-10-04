#include "dal/IO.h"
#include "dal/exceptions.h"
#include "ImportTableBuilder.h"

#include <fstream>
#include <iterator>
#include <stdexcept>
using namespace std;

namespace dal {
namespace {
// Character-state CSV parser handles quoted delimiters, doubled quotes, and CRLF records.
class CsvParser {
public:
    vector<vector<string>> parse(const string& input) {
        lineNumbers_.clear();
        vector<vector<string>> rows;
        vector<string> row;
        string field;
        bool insideQuotes = false;
        bool justClosedQuote = false;
        bool fieldHasStarted = false;
        bool rowHasContent = false;
        size_t line = 1U, recordLine = 1U;

        for (size_t i = 0; i < input.size(); ++i) {
            const char ch = input[i];

            if (insideQuotes) {
                if (ch == '"') {
                    if (i + 1U < input.size() && input[i + 1U] == '"') {
                        field.push_back('"');
                        ++i;
                    } else {
                        insideQuotes = false;
                        justClosedQuote = true;
                    }
                } else {
                    field.push_back(ch);
                    if (ch == '\n') {
                        ++line;
                    }
                }
                continue;
            }

            if (justClosedQuote && ch != ',' && ch != '\r' && ch != '\n' && ch != ' ' && ch != '\t') {
                throw ParseError("CSV line " + to_string(line) + ": unexpected character after closing quote");
            }

            if (ch == '"') {
                if (fieldHasStarted || !field.empty()) {
                    throw ParseError("CSV line " + to_string(line) + ": quote inside unquoted field");
                }
                insideQuotes = true;
                fieldHasStarted = true;
                rowHasContent = true;
            } else if (ch == ',') {
                row.push_back(field);
                field.clear();
                fieldHasStarted = false;
                justClosedQuote = false;
                rowHasContent = true;
            } else if (ch == '\n' || ch == '\r') {
                if (ch == '\r' && i + 1U < input.size() && input[i + 1U] == '\n') {
                    ++i;
                }
                row.push_back(field);
                rows.push_back(move(row));
                lineNumbers_.push_back(recordLine);

                row.clear();
                field.clear();
                fieldHasStarted = false;
                justClosedQuote = false;
                rowHasContent = false;
                ++line;
                recordLine = line;
            } else {
                if (justClosedQuote && (ch == ' ' || ch == '\t')) {
                    continue;
                }
                field.push_back(ch);
                fieldHasStarted = true;
                rowHasContent = true;
            }
        }

        if (insideQuotes) {
            throw ParseError("CSV line " + to_string(line) + ": unterminated quoted field");
        }
        if (rowHasContent || !row.empty() || !field.empty() || justClosedQuote) {
            row.push_back(field);
            rows.push_back(move(row));
            lineNumbers_.push_back(recordLine);
        }
        return rows;
    }
    const vector<size_t>& lineNumbers() const {
        return lineNumbers_;
    }
private:
    vector<size_t> lineNumbers_;
};
}

DataSet CsvImporter::load(const string& path) {
    ifstream input(path, ios::binary);
    if (!input) {
        throw FileError("Could not open CSV file: " + path);
    }

    const string content((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
    if (content.empty()) {
        throw ParseError("CSV line 1: empty file has no header");
    }

    CsvParser parser;
    const auto records = parser.parse(content);
    if (records.empty() || records.front().empty()) {
        throw ParseError("CSV line 1: missing header row");
    }

    const auto& headers = records.front();
    for (const auto& header : headers) {
        if (header.empty()) {
            throw ParseError("CSV line 1: header name cannot be empty");
        }
    }

    vector<vector<ImportedCell>> rows;
    for (size_t r = 1; r < records.size(); ++r) {
        if (records[r].size() != headers.size()) {
            throw ParseError("CSV line " + to_string(parser.lineNumbers()[r]) + ": expected " + to_string(headers.size()) + " fields, got " + to_string(records[r].size()));
        }

        vector<ImportedCell> values;
        for (const auto& text : records[r]) {
            if (text.empty()) {
                values.push_back(ImportedCell{nullopt, false});
            } else {
                values.push_back(ImportedCell{text, false});
            }
        }
        rows.push_back(move(values));
    }

    try {
        return ImportTableBuilder::build(headers, rows);
    } catch (const exception& error) {
        throw ParseError(string("CSV line 1: ") + error.what());
    }
}
}
