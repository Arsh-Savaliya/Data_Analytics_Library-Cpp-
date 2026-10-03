cmake_minimum_required(VERSION 3.16)
project(DataAnalyticsLibrary LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(dal
    src/MeanAnalyzer.cpp
    src/MedianAnalyzer.cpp
    src/DataSet.cpp
    src/ComparisonFilter.cpp
    src/AndFilter.cpp
    src/OrFilter.cpp
    src/NotFilter.cpp
    src/CsvImporter.cpp
    src/CsvExporter.cpp
    src/JsonExporter.cpp
    src/HistogramViz.cpp
    src/IoFactory.cpp
)
target_include_directories(dal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(dal PRIVATE -Wall -Wextra)

add_executable(dal_demo src/main.cpp)
target_link_libraries(dal_demo PRIVATE dal)
target_compile_options(dal_demo PRIVATE -Wall -Wextra)

enable_testing()
add_executable(test_core tests/test_core.cpp)
target_link_libraries(test_core PRIVATE dal)
target_compile_options(test_core PRIVATE -Wall -Wextra)
add_test(NAME core_tests COMMAND test_core)
