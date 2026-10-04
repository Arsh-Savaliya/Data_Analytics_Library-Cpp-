cmake_minimum_required(VERSION 3.16)
project(DataAnalyticsLibrary LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(dal
    src/NumericColumnAnalyzer.cpp
    src/MeanAnalyzer.cpp
    src/MedianAnalyzer.cpp
    src/ModeAnalyzer.cpp
    src/StdDevAnalyzer.cpp
    src/VarianceAnalyzer.cpp
    src/MinAnalyzer.cpp
    src/MaxAnalyzer.cpp
    src/SumAnalyzer.cpp
    src/CountAnalyzer.cpp
    src/PercentileAnalyzer.cpp
    src/CorrelationAnalyzer.cpp
    src/CovarianceAnalyzer.cpp
    src/AnalyzerFactory.cpp
    src/DataSet.cpp
    src/GroupedDataSet.cpp
    src/SummaryReport.cpp
    src/AnalyzerDecorator.cpp
    src/CorrelationMatrixViz.cpp
    src/CommandShell.cpp
    src/ComparisonFilter.cpp
    src/AndFilter.cpp
    src/OrFilter.cpp
    src/NotFilter.cpp
    src/CsvImporter.cpp
    src/JsonImporter.cpp
    src/CsvExporter.cpp
    src/JsonExporter.cpp
    src/HistogramViz.cpp
    src/BarChartViz.cpp
    src/BoxSummaryViz.cpp
    src/IoFactory.cpp
    src/Pipeline.cpp
)
target_include_directories(dal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(dal PRIVATE -Wall -Wextra)

add_executable(dal_demo demo/main.cpp)
target_link_libraries(dal_demo PRIVATE dal)
target_compile_options(dal_demo PRIVATE -Wall -Wextra)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

enable_testing()
add_executable(test_core tests/test_core.cpp)
target_link_libraries(test_core PRIVATE dal)
target_compile_options(test_core PRIVATE -Wall -Wextra)
add_test(NAME core_tests COMMAND test_core)

add_executable(test_analytics tests/test_analytics.cpp)
target_link_libraries(test_analytics PRIVATE dal)
target_compile_options(test_analytics PRIVATE -Wall -Wextra)
add_test(NAME analytics_tests COMMAND test_analytics)

add_executable(test_query_io tests/test_query_io.cpp)
target_link_libraries(test_query_io PRIVATE dal)
target_compile_options(test_query_io PRIVATE -Wall -Wextra)
add_test(NAME query_io_tests COMMAND test_query_io)

add_executable(test_phase3 tests/test_phase3.cpp)
target_link_libraries(test_phase3 PRIVATE dal)
target_compile_options(test_phase3 PRIVATE -Wall -Wextra)
add_test(NAME phase3_tests COMMAND test_phase3)

add_executable(performance_test tests/performance_test.cpp)
target_link_libraries(performance_test PRIVATE dal)
target_compile_options(performance_test PRIVATE -Wall -Wextra)
