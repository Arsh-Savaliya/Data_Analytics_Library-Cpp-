#include "dal/IO.h"
#include "dal/exceptions.h"
#include <cctype>
using namespace std;

namespace dal {
string IoFactory::extensionOf(const string& path) {
    const auto slash = path.find_last_of("/\\");
    const auto dot = path.find_last_of('.');
    if (dot == string::npos || (slash != string::npos && dot < slash)) { return {}; }
    string extension = path.substr(dot + 1U);
    for (char& character : extension) {
        character = static_cast<char>(tolower(static_cast<unsigned char>(character)));
    }
    return extension;
}
unique_ptr<IImporter> IoFactory::makeImporter(const string& filePath) {
    const auto extension = IoFactory::extensionOf(filePath);
    if (extension == "csv") {
        return make_unique<CsvImporter>();
    }
    if (extension == "json") {
        return make_unique<JsonImporter>();
    }
    throw FileError("Unsupported importer extension: " + extension);
}
unique_ptr<IExporter> IoFactory::makeExporter(const string& filePath) {
    const auto extension = IoFactory::extensionOf(filePath);
    if (extension == "csv") {
        return make_unique<CsvExporter>();
    }
    if (extension == "json") {
        return make_unique<JsonExporter>();
    }
    throw FileError("Unsupported exporter extension: " + extension);
}
}
