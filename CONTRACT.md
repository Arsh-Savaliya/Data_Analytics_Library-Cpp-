# Team contracts

All public declarations live under `include/dal/`; implementations and the demo live under `src/`.

- **Person A — Core Data:** owns `include/dal/ColumnBase.h`, `include/dal/Column.h`, `include/dal/DataSet.h`, and `include/dal/exceptions.h`.
- **Person B — Analytics + Visualizer:** owns `include/dal/IAnalyzer.h` and `include/dal/Visualizer.h`.
- **Person C — Query + Import/Export:** owns `include/dal/IFilter.h`, `include/dal/Filters.h`, and `include/dal/IO.h`.

**Never edit another person's header without telling the team.** Coordinate shared API changes first, then update implementations and this contract if ownership changes.
