#pragma once

#include "dal/DataSet.h"

#include <memory>
#include <ostream>
#include <string>
using namespace std;

namespace dal {

// Demonstrates abstraction for interchangeable input formats.
class IImporter {
public:
    virtual ~IImporter() = default;
    virtual DataSet load(const string& path) = 0;
};

// Demonstrates abstraction for interchangeable output formats.
class IExporter {
public:
    virtual ~IExporter() = default;
    virtual void save(const DataSet& data, const string& path) = 0;
};

// Demonstrates polymorphism for CSV input.
class CsvImporter final : public IImporter {
public:
    DataSet load(const string& path) override;
};

// Demonstrates polymorphism for CSV output.
class CsvExporter final : public IExporter {
public:
    void save(const DataSet& data, const string& path) override;
private:
    void writeField(ostream& output, const string& value) const;
};

// Demonstrates polymorphism for JSON output.
class JsonExporter final : public IExporter {
public:
    void save(const DataSet& data, const string& path) override;
private:
    void writeString(ostream& output, const string& value) const;
};

// Demonstrates format polymorphism for flat JSON row arrays.
class JsonImporter final : public IImporter {
public:
    DataSet load(const string& path) override;
};

// Demonstrates factory encapsulation for format-specific object creation.
class IoFactory {
public:
    static unique_ptr<IImporter> makeImporter(const string& filePath);
    static unique_ptr<IExporter> makeExporter(const string& filePath);
private:
    static string extensionOf(const string& path);
};

// Acronym-style spelling used by the integration demo; IoFactory remains available for compatibility.
using IOFactory = IoFactory;

} // namespace dal
