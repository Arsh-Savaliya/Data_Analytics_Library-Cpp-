#include "dal/Column.h"
#include "dal/DataSet.h"
#include "dal/IFilter.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
using namespace std;

namespace {
class PositiveFilter final : public dal::IFilter {
public:
    bool matches(const dal::DataSet& data, size_t row) const override {
        const auto& value = data.getColumnAs<int>("age").at(row);
        return value.has_value() && *value > 0;
    }
    unique_ptr<dal::IFilter> clone() const override { return make_unique<PositiveFilter>(*this); }
};
}

int main() {
    dal::Column<int> empty("empty");
    assert(empty.size() == 0U);
    assert(empty.countMissing() == 0U);

    dal::Column<int> values("values");
    values.addValue(3);
    values.addMissing();
    values.addValue(1);
    assert(values.size() == 3U);
    assert(values.countMissing() == 1U);
    assert(!values.at(1).has_value());
    assert(values.sorted().at(0).value() == 1);
    assert(values.unique().size() == 3U);
    values.set(0, 4);
    assert(values.at(0).value() == 4);

    const auto mask = values > dal::Column<int>("threshold", {2, nullopt, 2});
    assert(mask.at(0).value());
    assert(!mask.at(1).has_value());
    const auto plus = values + 2;
    assert(plus.at(0).value() == 6);
    assert(!plus.at(1).has_value());

    bool divisionByZeroRejected = false;
    try { (void)(values / 0); }
    catch (const domain_error&) { divisionByZeroRejected = true; }
    assert(divisionByZeroRejected);

    bool integerOverflowRejected = false;
    dal::Column<int> largestInt("largest", {numeric_limits<int>::max()});
    try { (void)(largestInt + 1); }
    catch (const overflow_error&) { integerOverflowRejected = true; }
    assert(integerOverflowRejected);

    bool boolConversionRejected = false;
    dal::Column<bool> flags("flags", {true});
    try { (void)flags.toDouble(0); }
    catch (const dal::TypeMismatch&) { boolConversionRejected = true; }
    assert(boolConversionRejected);

    dal::DataSet data;
    auto ages = make_unique<dal::Column<int>>("age");
    ages->addValue(19);
    ages->addValue(20);
    data.addColumn(move(ages));
    auto names = make_unique<dal::Column<string>>("name");
    names->addValue("Ada");
    names->addMissing();
    data.addColumn(move(names));

    bool duplicateRejected = false;
    try { data.addColumn(make_unique<dal::Column<int>>("age", vector<optional<int>>{1, 2})); }
    catch (const invalid_argument&) { duplicateRejected = true; }
    assert(duplicateRejected);

    bool sizeRejected = false;
    try { data.addColumn(make_unique<dal::Column<int>>("short", vector<optional<int>>{1})); }
    catch (const invalid_argument&) { sizeRejected = true; }
    assert(sizeRejected);

    dal::DataSet copied(data);
    copied.getColumnAs<int>("age").set(0, 99);
    assert(data.getColumnAs<int>("age").at(0).value() == 19);
    dal::DataSet assigned;
    assigned = data;
    assigned.getColumnAs<int>("age").set(0, 88);
    assert(data.getColumnAs<int>("age").at(0).value() == 19);

    bool typeRejected = false;
    try { (void)data.getColumnAs<double>("age"); }
    catch (const dal::TypeMismatch&) { typeRejected = true; }
    assert(typeRejected);
    bool stringNumericRejected = false;
    try { (void)data.getColumn("name").toDouble(0); }
    catch (const dal::TypeMismatch&) { stringNumericRejected = true; }
    assert(stringNumericRejected);

    const auto transformed = data.filterBy(PositiveFilter{}).select({"name", "age"}).head(1);
    assert(transformed.rowCount() == 1U);
    assert(transformed.getColumnAs<string>("name").at(0).value() == "Ada");
    assert(data.tail(1).getColumnAs<int>("age").at(0).value() == 20);
    assert(data.select({}).rowCount() == data.rowCount());
    return 0;
}
