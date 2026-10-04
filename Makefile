CXX ?= g++
AR ?= ar
CXXFLAGS ?= -std=c++17 -Wall -Wextra
CPPFLAGS ?= -Iinclude
BUILD_DIR := build
OBJECT_DIR := $(BUILD_DIR)/obj
LIBRARY := $(BUILD_DIR)/libdal.a

LIB_SOURCES := $(wildcard src/*.cpp)
LIB_OBJECTS := $(patsubst src/%.cpp,$(OBJECT_DIR)/%.o,$(LIB_SOURCES))
HEADERS := $(wildcard include/dal/*.h)

.PHONY: all demo run tests test performance clean

all: demo tests performance

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(OBJECT_DIR)/%.o: src/%.cpp $(HEADERS) | $(OBJECT_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(LIBRARY): $(LIB_OBJECTS)
	$(AR) rcs $@ $^

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

demo: $(BUILD_DIR)/dal_demo

$(BUILD_DIR)/dal_demo: demo/main.cpp $(LIBRARY) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(LIBRARY) -o $@

run: demo
	./$(BUILD_DIR)/dal_demo

tests: $(BUILD_DIR)/test_core $(BUILD_DIR)/test_analytics $(BUILD_DIR)/test_query_io $(BUILD_DIR)/test_phase3

$(BUILD_DIR)/test_core: tests/test_core.cpp $(LIBRARY) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(LIBRARY) -o $@

$(BUILD_DIR)/test_analytics: tests/test_analytics.cpp $(LIBRARY) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(LIBRARY) -o $@

$(BUILD_DIR)/test_query_io: tests/test_query_io.cpp $(LIBRARY) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(LIBRARY) -o $@

$(BUILD_DIR)/test_phase3: tests/test_phase3.cpp $(LIBRARY) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(LIBRARY) -o $@

test: tests
	./$(BUILD_DIR)/test_core
	./$(BUILD_DIR)/test_analytics
	./$(BUILD_DIR)/test_query_io
	./$(BUILD_DIR)/test_phase3

performance: $(BUILD_DIR)/performance_test

$(BUILD_DIR)/performance_test: tests/performance_test.cpp $(LIBRARY) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(LIBRARY) -o $@

clean:
	rm -rf $(BUILD_DIR)
