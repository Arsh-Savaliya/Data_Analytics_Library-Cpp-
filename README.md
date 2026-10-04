# Data Analytics Library

A beginner-oriented C++17 library for working with typed tabular data. The project demonstrates object-oriented design through typed columns, analyzer strategies, composable filters, interchangeable file formats, visualizers, and an ordered processing pipeline.

## Architecture overview

| Component | Responsibility | Main files |
|---|---|---|
| Core data | Store typed values and manage rows and columns | `include/dal/ColumnBase.h`, `include/dal/Column.h`, `include/dal/DataSet.h`, `src/DataSet.cpp` |
| Analytics | Calculate summary and paired statistics | `include/dal/IAnalyzer.h`, analyzer headers in `include/dal/`, analyzer sources in `src/` |
| Visualizers | Render numeric and category summaries | `include/dal/Visualizer.h`, `src/HistogramViz.cpp`, `src/BarChartViz.cpp`, `src/BoxSummaryViz.cpp` |
| Query | Match rows using simple or combined conditions | `include/dal/IFilter.h`, `include/dal/Filters.h`, `src/AndFilter.cpp`, `src/OrFilter.cpp`, `src/NotFilter.cpp` |
| File I/O | Read CSV/JSON and write CSV/JSON | `include/dal/IO.h`, `src/CsvImporter.cpp`, `src/JsonImporter.cpp`, `src/CsvExporter.cpp`, `src/JsonExporter.cpp` |
| Pipeline | Run filter, select, analysis, visualization, and export steps in order | `include/dal/Pipeline.h`, `src/Pipeline.cpp` |

`DataSet` owns its columns with `unique_ptr<ColumnBase>`. A `Column<T>` keeps values of one type in a vector of optional values. The base interface lets the dataset and analytics code work with a column without knowing its concrete type in advance.

## Course concepts and where to find them

| Concept | Example classes | Files and explanation |
|---|---|---|
| Abstraction | `ColumnBase`, `IAnalyzer`, `IFilter`, `IImporter`, `IExporter`, `Visualizer`, `IStep` | `include/dal/ColumnBase.h`, `IAnalyzer.h`, `IFilter.h`, `IO.h`, `Visualizer.h`, `Pipeline.h`; each defines a contract for other classes. |
| Inheritance | `Column<T>`, `MeanAnalyzer`, `AndFilter`, `CsvImporter`, `HistogramViz`, `FilterStep` | These derive from an abstract base or interface and provide concrete behavior. See their headers under `include/dal/`. |
| Polymorphism | `DataSet`, `AnalyzerFactory`, `Pipeline` | `src/DataSet.cpp`, `src/AnalyzerFactory.cpp`, `src/Pipeline.cpp`; virtual calls dispatch to the stored concrete column, analyzer, filter, or step. |
| Virtual functions | All interface base classes | Each base declares virtual operations and has a virtual destructor; implementations use `override`. |
| Templates | `Column<T>`, `ComparisonFilter<T>`, `RangeFilter<T>` | `include/dal/Column.h` and `include/dal/Filters.h`; one implementation works with multiple value types. |
| Encapsulation | `Column<T>`, `DataSet`, `Pipeline` | Private vectors and pointers hide ownership and stored values; callers use public methods. |
| Strategy pattern | `IAnalyzer`, `Visualizer`, `IImporter`, `IExporter` | Implementations can be selected without changing the calling code. |
| Composite pattern | `AndFilter`, `OrFilter`, `NotFilter` | `include/dal/Filters.h` and the matching source files; simple filters can be nested. |
| Factory pattern | `AnalyzerFactory`, `IOFactory` | `src/AnalyzerFactory.cpp` and `src/IoFactory.cpp` create implementations based on a name or file extension. |
| Decorator pattern | `AnalyzerDecorator`, `LoggingAnalyzer`, `RoundingAnalyzer` | `include/dal/AnalyzerDecorator.h` wraps an analyzer while preserving its interface. |
| Command pattern | `ICommand`, `CommandShell` | `include/dal/CommandShell.h` and `src/CommandShell.cpp` turn parsed requests into command objects. |
| Composition and ownership | `DataSet`, `Pipeline`, composite filters | Each owns child columns, steps, or filters through `unique_ptr`. |

## Phase 3 extensions

`DataSet` now supports deep copy and move, stable sorting, inner joins, and grouped
aggregates (`groupBy(...).agg("mean", ...)`). `SummaryReport` creates a describe-style
table, `AnalyzerDecorator` adds logging and rounding, and `CorrelationMatrixViz`
prints an ASCII correlation matrix. `CommandShell` turns text commands into polymorphic
command objects. The `performance_test` target times a generated 100,000-row CSV.

## Build and run

Configure and build from the repository root:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Alternatively, with GNU Make installed, use `make`, `make run`, `make test`, or
`make performance`. Run `make clean` to remove generated build files.

Run the demo from the repository root:

```sh
./build/dal_demo
```

On Windows, the executable may be under `build/Debug/dal_demo.exe` or `build/Release/dal_demo.exe`, depending on the generator. The demo reads `data/students.csv` and writes filtered CSV and JSON files in `data/`.

## Small usage example

```cpp
#include "dal/Filters.h"
#include "dal/IO.h"

using namespace std;
using namespace dal;

int main() {
    unique_ptr<IImporter> importer = IOFactory::makeImporter("data/students.csv");
    DataSet students = importer->load("data/students.csv");

    FilterHandle condition = gt("gpa", 8.0) && lt("age", 22);
    DataSet honors = students.filterBy(condition).select({"name", "gpa"});
    honors.printTable();
}
```

The full example is in `demo/main.cpp`. It loads the sample, prints the original table, filters and selects rows, calculates summaries, renders a histogram and category chart, and exports the result through a `Pipeline`.

## Demo menu

Run the demo and enter a menu number to view the table, summary, GPA histogram, city bar chart, GPA box summary, or correlation matrix. Enter `0` to exit.

## UML class diagram

```mermaid
classDiagram
    class ColumnBase {
      <<abstract>>
      +getName() string
      +size() size_t
      +print() void
      +clone() unique_ptr~ColumnBase~
      +isNumeric() bool
      +toDouble(i) double
      +isMissing(i) bool
      +typeName() string
      +copyRows(rows) unique_ptr~ColumnBase~
    }
    class Column~T~ {
      -name_ string
      -data_ vector~optional~T~~
      +addValue(value) void
      +addMissing() void
      +at(i) optional~T~
      +set(i,value) void
      +sorted() Column~T~
      +slice(from,to) Column~T~
      +unique() Column~T~
    }
    class DataSet {
      -columns_ vector~unique_ptr~ColumnBase~~
      -rowCount_ size_t
      +addColumn(column) void
      +removeColumn(name) void
      +getColumn(name) ColumnBase
      +getColumnAs~T~(name) Column~T~
      +filterBy(filter) DataSet
      +select(names) DataSet
      +head(n) DataSet
      +tail(n) DataSet
      +printTable() void
    }
    class IAnalyzer {
      <<interface>>
      +analyze(column) double
      +name() string
    }
    class MeanAnalyzer
    class MedianAnalyzer
    class ModeAnalyzer
    class StdDevAnalyzer
    class VarianceAnalyzer
    class MinAnalyzer
    class MaxAnalyzer
    class SumAnalyzer
    class CountAnalyzer
    class PercentileAnalyzer
    class IPairAnalyzer {
      <<interface>>
      +analyze(first,second) double
    }
    class CorrelationAnalyzer
    class CovarianceAnalyzer
    class AnalyzerFactory
    class IFilter {
      <<interface>>
      +matches(data,row) bool
      +clone() unique_ptr~IFilter~
    }
    class ComparisonFilter~T~
    class StringContainsFilter
    class RangeFilter~T~
    class IsMissingFilter
    class AndFilter
    class OrFilter
    class NotFilter
    class IImporter {
      <<interface>>
      +load(path) DataSet
    }
    class IExporter {
      <<interface>>
      +save(data,path) void
    }
    class CsvImporter
    class JsonImporter
    class CsvExporter
    class JsonExporter
    class IOFactory
    class Visualizer {
      <<abstract>>
      +render(column,output) void
    }
    class HistogramViz
    class BarChartViz
    class BoxSummaryViz
    class IStep {
      <<interface>>
      +run(data,output) void
    }
    class FilterStep
    class SelectStep
    class AnalyzeStep
    class VisualizeStep
    class ExportStep
    class Pipeline

    ColumnBase <|-- Column~T~
    IAnalyzer <|.. MeanAnalyzer
    IAnalyzer <|.. MedianAnalyzer
    IAnalyzer <|.. ModeAnalyzer
    IAnalyzer <|.. StdDevAnalyzer
    IAnalyzer <|.. VarianceAnalyzer
    IAnalyzer <|.. MinAnalyzer
    IAnalyzer <|.. MaxAnalyzer
    IAnalyzer <|.. SumAnalyzer
    IAnalyzer <|.. CountAnalyzer
    IAnalyzer <|.. PercentileAnalyzer
    IPairAnalyzer <|.. CorrelationAnalyzer
    IPairAnalyzer <|.. CovarianceAnalyzer
    IFilter <|.. ComparisonFilter~T~
    IFilter <|.. StringContainsFilter
    IFilter <|.. RangeFilter~T~
    IFilter <|.. IsMissingFilter
    IFilter <|.. AndFilter
    IFilter <|.. OrFilter
    IFilter <|.. NotFilter
    IImporter <|.. CsvImporter
    IImporter <|.. JsonImporter
    IExporter <|.. CsvExporter
    IExporter <|.. JsonExporter
    Visualizer <|-- HistogramViz
    Visualizer <|-- BarChartViz
    Visualizer <|-- BoxSummaryViz
    IStep <|.. FilterStep
    IStep <|.. SelectStep
    IStep <|.. AnalyzeStep
    IStep <|.. VisualizeStep
    IStep <|.. ExportStep
    DataSet *-- "0..*" ColumnBase : owns
    AndFilter *-- "2" IFilter : owns
    OrFilter *-- "2" IFilter : owns
    NotFilter *-- "1" IFilter : owns
    Pipeline *-- "0..*" IStep : owns ordered steps
    FilterStep o-- IFilter : owns
    AnalyzeStep ..> AnalyzerFactory : uses
    VisualizeStep o-- Visualizer : owns
    ExportStep o-- IExporter : owns
    IOFactory ..> IImporter : creates
    IOFactory ..> IExporter : creates
    AnalyzerFactory ..> IAnalyzer : creates
    DataSet ..> IFilter : evaluates
    IAnalyzer ..> ColumnBase : analyzes
```
