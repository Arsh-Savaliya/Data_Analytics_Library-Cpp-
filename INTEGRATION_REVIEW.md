# Integration review

## Module boundaries checked

| Interaction | Check performed |
|---|---|
| `Column<T>` → `ColumnBase` | Virtual methods preserve the concrete type through `clone()` and `copyRows()`. Missing cells are checked before numeric conversion. `Column<bool>` is not numeric, and `toDouble()` rejects it consistently. |
| `DataSet` → `ColumnBase` | Columns are owned by `unique_ptr`; deep copies call virtual `clone()`. Row operations pass selected indices to `copyRows()`. `getColumn()` references are used immediately, not stored. |
| `DataSet` → `IFilter` | `filterBy()` calls the filter for each existing row and copies selected rows. `ComparisonFilter<T>` uses `getColumnAs<T>()`; missing values never match. Composite filters own and deep-clone their children. |
| Analytics → `ColumnBase` | Single-column analyzers check `isNumeric()`, skip `isMissing(i)`, and read with `toDouble(i)`. Pair analyzers validate equal lengths and use complete pairs only. Sample calculations check that enough values exist. |
| Visualizers → `ColumnBase` | Numeric plots check `isNumeric()` and skip missing entries; category charts use `valueAsString()`. Visualizers read through const interfaces and write to a caller-provided stream. |
| Importers → `DataSet` / `Column<T>` | Importers return datasets by value and construct typed columns. CSV validates record widths; JSON accepts flat arrays of row objects. File and parse failures use project exceptions. |
| Exporters → `DataSet` / `ColumnBase` | Exporters receive `const DataSet&`, escape strings, and output missing values as empty CSV cells or JSON `null`. Double text formatting retains enough precision for round trips. |
| Factories → interfaces | `AnalyzerFactory` creates fresh `unique_ptr<IAnalyzer>` objects. `IOFactory` (also named `IoFactory` for compatibility) creates importer/exporter interface pointers based on a filename extension. |
| Pipeline → steps and modules | `Pipeline` owns ordered `unique_ptr<IStep>` objects. It passes a deep copy through the steps. Each step uses the filter, analyzer, visualizer, or exporter interface and stores no column references. |
| Build → modules | CMake includes all module and test sources. It copies the sample data folder into the build directory at configure time. |

## Fixes made during integration

- Added Rule of Five move operations and retained deep-copy independence in `DataSet`.
- Added row-index-preserving sort and inner join; missing join keys are excluded, and colliding right-side names receive a `right_` prefix.
- Grouped aggregations copy only rows for one group and dispatch through `AnalyzerFactory`.
- Summary reports own a snapshot of the dataset; decorators own their wrapped analyzer through `unique_ptr`.
- The command shell parses input into short-lived `ICommand` implementations and applies transformations by replacing its owned dataset.
- A 15-case malformed CSV set now verifies the importer consistently uses `ParseError`.

- Made `Column<bool>::toDouble()` reject conversion, matching `isNumeric()`.
- Added division-by-zero checks and signed integer overflow checks for division, addition, subtraction, and multiplication in `Column<int>` arithmetic.
- Set double text output precision high enough to avoid losing values during CSV/JSON export and re-import.
- Preserved row counts for transformations such as `select({})`, which return zero columns but still represent existing rows.
- Made the summary step print `NA (no values)` for all-missing numeric columns instead of aborting the pipeline.
- Changed sample GPA values to a 10-point scale so `gpa > 8 AND age < 22` selects rows in the demo.

## Ownership, virtual functions, and indexing

Every polymorphic base (`ColumnBase`, `IAnalyzer`, `IPairAnalyzer`, `IFilter`, `IImporter`, `IExporter`, `Visualizer`, and `IStep`) has a virtual destructor. Stored polymorphic children use `unique_ptr`; there are no owning raw pointers. Overrides use `override`. Analytics and visualizers read const column references. Dataset references are not retained by the pipeline. Loops use `row < size()`, and typed-column access uses bounds-checked `at()`.

`DataSet::getColumn()` references become invalid if the column is removed or its dataset is destroyed. Callers must not retain them across those operations; the demo uses each reference immediately.

## Runtime verification limits

The demo and test programs compile and run with `-std=c++17 -Wall -Wextra`. An AddressSanitizer/UBSan build was attempted with `-fsanitize=address,undefined`, but linking failed because this MinGW toolchain does not include `libasan` or `libubsan`. Valgrind and CMake are also not installed in the current environment. A manual Valgrind-style ownership review found no leaked owning allocations: polymorphic ownership is held by `unique_ptr`, and temporary streams, vectors, and parser values are automatic objects. No out-of-bounds access or use-after-free was found in the demo path; this reasoning is not a substitute for running the missing tools.
