#include "dal/IO.h"
#include "dal/exceptions.h"
#include "ImportTableBuilder.h"

#include <cstdint>
#include <fstream>
#include <iterator>
#include <map>
using namespace std;

namespace dal {
namespace {
struct JsonScalar {
    bool isNull = false;
    bool isString = false;
    string value;
};
using JsonObject = vector<pair<string, JsonScalar>>;

// Small recursive-descent parser constrained to a top-level array of flat objects.
class FlatJsonParser {
public:
    explicit FlatJsonParser(const string& input) : input_(input) {}

    vector<JsonObject> parse() {
        skipSpace();
        expect('[');

        vector<JsonObject> rows;
        skipSpace();
        if (consume(']')) {
            finish();
            return rows;
        }

        while (true) {
            rows.push_back(parseObject());
            skipSpace();
            if (consume(']')) {
                break;
            }
            expect(',');
        }

        finish();
        return rows;
    }

private:
    // Reads one object. Nested objects are not accepted because this importer is for flat rows.
    JsonObject parseObject() {
        skipSpace();
        expect('{');
        skipSpace();

        JsonObject fields;
        if (consume('}')) {
            return fields;
        }

        while (true) {
            skipSpace();
            if (peek() != '"') {
                fail("object key must be a string");
            }

            const string key = parseString();
            skipSpace();
            expect(':');
            skipSpace();

            for (const auto& field : fields) {
                if (field.first == key) {
                    fail("duplicate object key");
                }
            }

            fields.emplace_back(move(key), parseScalar());
            skipSpace();
            if (consume('}')) {
                break;
            }
            expect(',');
        }
        return fields;
    }

    // A row cell may be a string, number, boolean, or null.
    JsonScalar parseScalar() {
        skipSpace();
        if (peek() == '"') {
            return JsonScalar{false, true, parseString()};
        }
        if (consumeWord("null")) {
            return JsonScalar{true, false, {}};
        }
        if (consumeWord("true")) {
            return JsonScalar{false, true, "true"};
        }
        if (consumeWord("false")) {
            return JsonScalar{false, true, "false"};
        }

        const auto start = position_;
        if (peek() == '-') {
            advance();
        }
        if (peek() == '0') {
            advance();
        } else {
            if (!isDigit(peek())) {
                fail("expected scalar value");
            }
            while (isDigit(peek())) {
                advance();
            }
        }

        if (peek() == '.') {
            advance();
            if (!isDigit(peek())) {
                fail("invalid number fraction");
            }
            while (isDigit(peek())) {
                advance();
            }
        }

        if (peek() == 'e' || peek() == 'E') {
            advance();
            if (peek() == '+' || peek() == '-') {
                advance();
            }
            if (!isDigit(peek())) {
                fail("invalid number exponent");
            }
            while (isDigit(peek())) {
                advance();
            }
        }

        return JsonScalar{false, false, input_.substr(start, position_ - start)};
    }

    // Reads a string and translates JSON's backslash escapes into regular characters.
    string parseString() {
        expect('"');
        string result;
        while (position_ < input_.size()) {
            const unsigned char ch = static_cast<unsigned char>(peek());
            advance();
            if (ch == '"') {
                return result;
            }
            if (ch < 0x20U) {
                fail("control character in string");
            }
            if (ch != '\\') {
                result.push_back(static_cast<char>(ch));
                continue;
            }
            if (position_ >= input_.size()) {
                fail("unterminated escape sequence");
            }
            const char escaped = peek();
            advance();
            switch (escaped) {
            case '"': result.push_back('"'); break;
            case '\\': result.push_back('\\'); break;
            case '/': result.push_back('/'); break;
            case 'b': result.push_back('\b'); break;
            case 'f': result.push_back('\f'); break;
            case 'n': result.push_back('\n'); break;
            case 'r': result.push_back('\r'); break;
            case 't': result.push_back('\t'); break;
            case 'u': {
                uint32_t codepoint = readHex4();
                if (codepoint >= 0xD800U && codepoint <= 0xDBFFU) {
                    if (position_ + 1U >= input_.size() ||
                        input_[position_] != '\\' || input_[position_ + 1U] != 'u') {
                        fail("missing low surrogate");
                    }
                    advance();
                    advance();
                    const uint32_t low = readHex4();
                    if (low < 0xDC00U || low > 0xDFFFU) {
                        fail("invalid low surrogate");
                    }
                    codepoint = 0x10000U + ((codepoint - 0xD800U) << 10U) + (low - 0xDC00U);
                } else if (codepoint >= 0xDC00U && codepoint <= 0xDFFFU) {
                    fail("unexpected low surrogate");
                }
                appendUtf8(result, codepoint);
                break;
            }
            default: fail("invalid string escape");
            }
        }
        fail("unterminated string");
        return result;
    }
    uint32_t readHex4() {
        uint32_t value = 0U;
        for (int i = 0; i < 4; ++i) {
            if (position_ >= input_.size()) {
                fail("short unicode escape");
            }
            const char ch = peek();
            advance();
            value <<= 4U;
            if (ch >= '0' && ch <= '9') {
                value += static_cast<uint32_t>(ch - '0');
            } else if (ch >= 'a' && ch <= 'f') {
                value += static_cast<uint32_t>(ch - 'a' + 10);
            } else if (ch >= 'A' && ch <= 'F') {
                value += static_cast<uint32_t>(ch - 'A' + 10);
            } else {
                fail("invalid unicode escape");
            }
        }
        return value;
    }
    void appendUtf8(string& output, uint32_t cp) {
        if (cp <= 0x7FU) {
            output.push_back(static_cast<char>(cp));
        } else if (cp <= 0x7FFU) {
            output.push_back(static_cast<char>(0xC0U | (cp >> 6U)));
            output.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
        } else if (cp <= 0xFFFFU) {
            output.push_back(static_cast<char>(0xE0U | (cp >> 12U)));
            output.push_back(static_cast<char>(0x80U | ((cp >> 6U) & 0x3FU)));
            output.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
        } else {
            output.push_back(static_cast<char>(0xF0U | (cp >> 18U)));
            output.push_back(static_cast<char>(0x80U | ((cp >> 12U) & 0x3FU)));
            output.push_back(static_cast<char>(0x80U | ((cp >> 6U) & 0x3FU)));
            output.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
        }
    }
    bool consumeWord(const string& word) {
        if (input_.compare(position_, word.size(), word) != 0) {
            return false;
        }
        for (size_t i = 0; i < word.size(); ++i) {
            advance();
        }
        return true;
    }
    void finish() {
        skipSpace();
        if (position_ != input_.size()) {
            fail("trailing content after array");
        }
    }

    void skipSpace() {
        while (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n') {
            advance();
        }
    }

    void expect(char expected) {
        if (!consume(expected)) {
            fail(string("expected '") + expected + "'");
        }
    }

    bool consume(char expected) {
        if (peek() != expected) {
            return false;
        }
        advance();
        return true;
    }

    char peek() const {
        if (position_ >= input_.size()) {
            return '\0';
        }
        return input_[position_];
    }

    void advance() {
        if (input_[position_] == '\n') {
            ++line_;
        }
        ++position_;
    }

    static bool isDigit(char ch) {
        return ch >= '0' && ch <= '9';
    }

    [[noreturn]] void fail(const string& message) const {
        throw ParseError("JSON line " + to_string(line_) + ": " + message);
    }
    const string& input_;
    size_t position_ = 0U;
    size_t line_ = 1U;
};
}

DataSet JsonImporter::load(const string& path) {
    ifstream input(path, ios::binary);
    if (!input) {
        throw FileError("Could not open JSON file: " + path);
    }

    const string content((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
    if (content.empty()) {
        throw ParseError("JSON line 1: empty file");
    }

    FlatJsonParser parser(content);
    const auto objects = parser.parse();
    if (objects.empty()) {
        return DataSet{};
    }

    vector<string> headers;
    for (const auto& field : objects.front()) {
        headers.push_back(field.first);
    }

    vector<vector<ImportedCell>> rows;
    for (size_t rowIndex = 0; rowIndex < objects.size(); ++rowIndex) {
        map<string, JsonScalar> fields;
        for (const auto& field : objects[rowIndex]) {
            fields.emplace(field.first, field.second);
        }
        if (fields.size() != headers.size()) {
            throw ParseError("JSON line " + to_string(rowIndex + 1U) + ": row object keys differ");
        }

        vector<ImportedCell> row;
        for (const auto& header : headers) {
            const auto found = fields.find(header);
            if (found == fields.end()) {
                throw ParseError("JSON line " + to_string(rowIndex + 1U) + ": row object keys differ");
            }

            const auto& cell = found->second;
            row.push_back(ImportedCell{cell.isNull ? nullopt : optional<string>(cell.value), cell.isString});
        }
        rows.push_back(move(row));
    }

    try {
        return ImportTableBuilder::build(headers, rows);
    } catch (const exception& error) {
        throw ParseError(string("JSON line 1: ") + error.what());
    }
}
}
