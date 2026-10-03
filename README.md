# Data Analytics Library
 
A C++17 object-oriented library skeleton for typed tabular data, interchangeable analyzers, composable filters, import/export strategies, and visualizers.

## Build and run

```sh
cmake -S . -B build
cmake --build build
./build/dal_demo
```

The implemented proof path creates a `DataSet`, adds a `Column<double>`, and computes its mean. Other API methods are intentional `not implemented` stubs for parallel team development. See [UML.md](UML.md) for the class diagram and [CONTRACT.md](CONTRACT.md) for header ownership.
