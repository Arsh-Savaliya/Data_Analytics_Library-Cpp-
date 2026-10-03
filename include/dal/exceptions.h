#pragma once

#include <stdexcept>
#include <string>

namespace dal {

// Demonstrates inheritance by specializing the standard runtime error contract.
class ColumnNotFound : public std::runtime_error {
public:
    explicit ColumnNotFound(const std::string& message) : std::runtime_error(message) {}
};

// Demonstrates inheritance by specializing the standard runtime error contract.
class TypeMismatch : public std::runtime_error {
public:
    explicit TypeMismatch(const std::string& message) : std::runtime_error(message) {}
};

// Demonstrates inheritance by specializing the standard runtime error contract.
class EmptyColumn : public std::runtime_error {
public:
    explicit EmptyColumn(const std::string& message) : std::runtime_error(message) {}
};

// Demonstrates inheritance by specializing the standard runtime error contract.
class FileError : public std::runtime_error {
public:
    explicit FileError(const std::string& message) : std::runtime_error(message) {}
};

// Demonstrates inheritance by specializing the standard runtime error contract.
class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& message) : std::runtime_error(message) {}
};

} // namespace dal
