#pragma once

#include "dal/DataSet.h"

#include <memory>
#include <string>

namespace dal {

// Demonstrates abstraction for interchangeable input formats.
class IImporter {
public:
    virtual ~IImporter() = default;
    virtual DataSet load(const std::string& path) = 0;
};

// Demonstrates abstraction for interchangeable output formats.
class IExporter {
public:
    virtual ~IExporter() = default;
    virtual void save(const DataSet& data, const std::string& path) = 0;
};

// Demonstrates polymorphism for CSV input.
class CsvImporter final : public IImporter {
public:
    DataSet load(const std::string& path) override;
};

// Demonstrates polymorphism for CSV output.
class CsvExporter final : public IExporter {
public:
    void save(const DataSet& data, const std::string& path) override;
};

// Demonstrates polymorphism for JSON output.
class JsonExporter final : public IExporter {
public:
    void save(const DataSet& data, const std::string& path) override;
};

// Demonstrates factory encapsulation for format-specific object creation.
class IoFactory {
public:
    static std::unique_ptr<IImporter> makeImporter(const std::string& format);
    static std::unique_ptr<IExporter> makeExporter(const std::string& format);
};

} // namespace dal
