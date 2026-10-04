# Final submission checklist

- [x] C++17 library target and a runnable demo.
- [x] Encapsulated typed columns with first-class missing values.
- [x] Abstraction, inheritance, polymorphism, virtual functions, and templates.
- [x] Dataset ownership, deep copying, row selection, filtering, and table display.
- [x] DataSet Rule of Five, sortBy, inner join, and groupBy aggregation with tests.
- [x] Analyzer strategies, pair analyzers, factory, and visualizers.
- [x] SummaryReport, analyzer decorators, and correlation matrix visualization with tests and demo use.
- [x] CommandShell with polymorphic commands; 15 malformed CSV cases assert ParseError.
- [x] 100,000-row performance test: current run loaded in 2,592 ms and filtered in 17 ms (54,000 matches).
- [x] Composite filters and expression builder.
- [x] CSV/JSON import and export with extension-based factory selection.
- [x] Ordered polymorphic pipeline demonstrated in `demo/main.cpp`.
- [x] Creative integration: one run combines filtering, analytics, histogram/bar views, and two export formats.
- [x] Sample CSV has 20 rows and intentional missing cells.
- [x] Core, analytics, and query/I/O tests are included in CMake.
- [x] Phase 3 behavior tests are included in CMake.
- [x] README has architecture, build steps, course-concept mapping, usage example, and Mermaid UML.
- [x] Integration review records module boundaries and applied fixes.
- [x] Manual Valgrind-style review of ownership, row bounds, and temporary lifetimes.
- [ ] ASan/UBSan execution: MinGW cannot link because `libasan` and `libubsan` are missing.
- [ ] Valgrind execution: Valgrind is not installed in this environment.

CMake is also unavailable here, so validation used direct `g++` commands with the same C++17 warning flags. The CMake targets are present for submission and can be run on a machine with CMake installed.

## Three creative extensions

1. Add rolling-window statistics and time-series columns.
2. Add SQL-like grouping and joins to the filter/query layer.
3. Add SVG or HTML visualizers that open in a browser.
