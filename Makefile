# CognitaOS Makefile

CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O3 -I./include -fPIC
CXXFLAGS = -Wall -Wextra -O3 -I./include -std=c++20
LDFLAGS = -shared

BUILD_DIR = build
LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin

SOURCES = $(wildcard src/kernel/*.c) \
          $(wildcard src/compiler/*.c) \
          $(wildcard src/runtime/*.c) \
          $(wildcard src/hal/*.c)

OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

TARGET = $(LIB_DIR)/libcognita.so

.PHONY: all clean test examples benchmarks install

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(LIB_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

test: all
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_attention_scheduler tests/unit/test_attention_scheduler.c -L$(LIB_DIR) -lcognita -lm
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_semantic_memory tests/unit/test_semantic_memory.c -L$(LIB_DIR) -lcognita -lm
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_context_switch tests/unit/test_context_switch.c -L$(LIB_DIR) -lcognita -lm
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_intent_compiler tests/unit/test_intent_compiler.c -L$(LIB_DIR) -lcognita -lm
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_agent_lifecycle tests/integration/test_agent_lifecycle.c -L$(LIB_DIR) -lcognita -lm
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_capability_chain tests/integration/test_capability_chain.c -L$(LIB_DIR) -lcognita -lm
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_semantic_store tests/integration/test_semantic_store.c -L$(LIB_DIR) -lcognita -lm
	@echo "Running tests..."
	@LD_LIBRARY_PATH=$(LIB_DIR) $(BIN_DIR)/test_attention_scheduler
	@LD_LIBRARY_PATH=$(LIB_DIR) $(BIN_DIR)/test_semantic_memory
	@LD_LIBRARY_PATH=$(LIB_DIR) $(BIN_DIR)/test_context_switch
	@LD_LIBRARY_PATH=$(LIB_DIR) $(BIN_DIR)/test_intent_compiler
	@LD_LIBRARY_PATH=$(LIB_DIR) $(BIN_DIR)/test_agent_lifecycle
	@LD_LIBRARY_PATH=$(LIB_DIR) $(BIN_DIR)/test_capability_chain
	@LD_LIBRARY_PATH=$(LIB_DIR) $(BIN_DIR)/test_semantic_store

examples: all
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/hello_intent examples/hello_intent.c -L$(LIB_DIR) -lcognita -lm
	$(CC) $(CFLAGS) -o $(BIN_DIR)/agent_orchestration examples/agent_orchestration.c -L$(LIB_DIR) -lcognita -lm
	$(CC) $(CFLAGS) -o $(BIN_DIR)/semantic_storage examples/semantic_storage.c -L$(LIB_DIR) -lcognita -lm
	$(CC) $(CFLAGS) -o $(BIN_DIR)/capability_chain examples/capability_chain.c -L$(LIB_DIR) -lcognita -lm

benchmarks: all
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/bench_context_switch tests/benchmarks/bench_context_switch.c -L$(LIB_DIR) -lcognita -lm
	$(CC) $(CFLAGS) -o $(BIN_DIR)/bench_semantic_query tests/benchmarks/bench_semantic_query.c -L$(LIB_DIR) -lcognita -lm
	$(CC) $(CFLAGS) -o $(BIN_DIR)/bench_intent_compile tests/benchmarks/bench_intent_compile.c -L$(LIB_DIR) -lcognita -lm

install: all
	@echo "Installing CognitaOS..."
	sudo mkdir -p /usr/local/lib
	sudo mkdir -p /usr/local/include/cognita
	sudo cp $(TARGET) /usr/local/lib/
	sudo cp include/cognita/*.h /usr/local/include/cognita/
	sudo ldconfig
	@echo "Installed."

clean:
	rm -rf $(BUILD_DIR)
