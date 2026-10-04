#pragma once

#include <stdexcept>
#include <string>
using namespace std;

namespace dal {

// Demonstrates inheritance by specializing the standard runtime error contract.
class ColumnNotFound : public runtime_error {
public:
    explicit ColumnNotFound(const string& message) : runtime_error(message) {}
};

// Demonstrates inheritance by specializing the standard runtime error contract.
class TypeMismatch : public runtime_error {
public:
    explicit TypeMismatch(const string& message) : runtime_error(message) {}
};

// Demonstrates inheritance by specializing the standard runtime error contract.
class EmptyColumn : public runtime_error {
public:
    explicit EmptyColumn(const string& message) : runtime_error(message) {}
};

// Demonstrates inheritance by specializing the standard runtime error contract.
class FileError : public runtime_error {
public:
    explicit FileError(const string& message) : runtime_error(message) {}
};

// Demonstrates inheritance by specializing the standard runtime error contract.
class ParseError : public runtime_error {
public:
    explicit ParseError(const string& message) : runtime_error(message) {}
};

} // namespace dal
