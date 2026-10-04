#include "dal/Column.h"
#include "dal/Filters.h"
#include "dal/IO.h"
#include "dal/exceptions.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
using namespace std;

namespace {
bool equalData(const dal::DataSet& left, const dal::DataSet& right) {
    if (left.columnNames() != right.columnNames() || left.rowCount() != right.rowCount()) { return false; }
    for (const auto& name : left.columnNames()) {
        const auto& a = left.getColumn(name);
        const auto& b = right.getColumn(name);
        if (a.typeName() != b.typeName()) { return false; }
        for (size_t row = 0; row < left.rowCount(); ++row) {
            if (a.isMissing(row) != b.isMissing(row)) { return false; }
            if (!a.isMissing(row) && a.valueAsString(row) != b.valueAsString(row)) { return false; }
        }
    }
    return true;
}
void writeFile(const filesystem::path& path, const string& content) {
    ofstream output(path, ios::binary);
    output << content;
}
}

int main() {
    const auto root = filesystem::temp_directory_path();
    const auto inputCsv = root / "dal_query_io_input.csv";
    const auto roundtripCsv = root / "dal_query_io_roundtrip.csv";
    const auto roundtripJson = root / "dal_query_io_roundtrip.json";
    const auto malformedCsv = root / "dal_query_io_malformed.csv";
    const auto emptyCsv = root / "dal_query_io_empty.csv";
    const auto headerCsv = root / "dal_query_io_header.csv";
    writeFile(inputCsv, "id,name,age,city\r\n1,\"Ada, Lovelace\",20,Bengaluru\r\n2,\"Zoë \"\"Z\"\"\",,München\r\n3,Grace,17,Delhi\r\n");

    dal::CsvImporter csvImporter;
    const auto original = csvImporter.load(inputCsv.string());
    assert(original.rowCount() == 3U);
    assert(original.getColumn("id").typeName() == "int");
    assert(original.getColumn("age").typeName() == "int");
    assert(original.getColumnAs<string>("name").at(0).value() == "Ada, Lovelace");
    assert(original.getColumnAs<string>("name").at(1).value() == "Zoë \"Z\"");
    assert(original.getColumn("age").isMissing(1));
    assert(original.getColumnAs<string>("city").at(1).value() == "München");

    const auto filter = dal::gt("age", 18) &&
        (dal::eq("city", string("Bengaluru")) || dal::eq("name", string("Grace")));
    const auto selected = original.filterBy(filter);
    assert(selected.rowCount() == 1U);
    assert(selected.getColumnAs<string>("name").at(0).value() == "Ada, Lovelace");
    const auto missing = original.filterBy(dal::isMissing("age"));
    assert(missing.rowCount() == 1U);

    dal::CsvExporter{}.save(original, roundtripCsv.string());
    const auto csvAgain = csvImporter.load(roundtripCsv.string());
    assert(equalData(original, csvAgain));
    dal::JsonExporter{}.save(original, roundtripJson.string());
    dal::JsonImporter jsonImporter;
    const auto jsonAgain = jsonImporter.load(roundtripJson.string());
    assert(equalData(original, jsonAgain));
    assert(jsonAgain.getColumnAs<string>("city").at(1).value() == "München");
    assert(equalData(original, dal::IoFactory::makeImporter(roundtripJson.string())->load(roundtripJson.string())));

    writeFile(malformedCsv, "a,b\n1,\"unterminated\n");
    bool malformedRejected = false;
    try { (void)csvImporter.load(malformedCsv.string()); }
    catch (const dal::ParseError& error) { malformedRejected = string(error.what()).find("line") != string::npos; }
    assert(malformedRejected);

    writeFile(emptyCsv, "");
    bool emptyRejected = false;
    try { (void)csvImporter.load(emptyCsv.string()); }
    catch (const dal::ParseError& error) { emptyRejected = string(error.what()).find("line 1") != string::npos; }
    assert(emptyRejected);

    writeFile(headerCsv, "id,name\r\n");
    const auto headerOnly = csvImporter.load(headerCsv.string());
    assert(headerOnly.columnCount() == 2U);
    assert(headerOnly.rowCount() == 0U);

    // A small malformed-input fuzz set checks that malformed records consistently fail as ParseError.
    const vector<string> malformedInputs{
        "", "a,,c\n1,2,3\n", ",b\n1,2\n", "a,b\n1\n", "a\n1,2\n",
        "a,b\n\"open\n", "a,b\n\"x\"tail,2\n", "a,b\n\"x\"y,2\n",
        "a,b\n1,2,3\n", "a,b,c\n1,2\n", "a\n\"x\"z\n", "a,b\n1,2\n3\n",
        "a,b\n1,2,\n", "a,b\n\"x\"\"y,2\n", "a,b\n\"x,2\n"
    };
    size_t malformedCount = 0U;
    for (size_t i = 0; i < malformedInputs.size(); ++i) {
        writeFile(malformedCsv, malformedInputs[i]);
        try { (void)csvImporter.load(malformedCsv.string()); }
        catch (const dal::ParseError&) { ++malformedCount; }
    }
    assert(malformedCount == malformedInputs.size());

    return 0;
}
