CC = gcc
# Strict compilation flags
CFLAGS = -std=c99 \
         -pedantic \
         -pedantic-errors \
         -Wall \
         -Wextra \
         -Wformat=2 \
         -Wformat-security \
         -Wnull-dereference \
         -Wstack-protector \
         -Wtrampolines \
         -Walloca \
         -Wvla \
         -Warray-bounds=2 \
         -Wimplicit-fallthrough=3 \
         -Wshift-overflow=2 \
         -Wcast-qual \
         -Wcast-align=strict \
         -Wconversion \
         -Wsign-conversion \
         -Wlogical-op \
         -Wduplicated-cond \
         -Wduplicated-branches \
         -Wrestrict \
         -Wnested-externs \
         -Winline \
         -Wundef \
         -Wstrict-prototypes \
         -Wmissing-prototypes \
         -Wmissing-declarations \
         -Wredundant-decls \
         -Wshadow \
         -Wwrite-strings \
         -Wfloat-equal \
         -Wpointer-arith \
         -Wbad-function-cast \
         -Wold-style-definition

# Security hardening flags
HARDENING = -D_FORTIFY_SOURCE=2 \
            -fstack-protector-strong \
            -fPIE \
            -fstack-clash-protection \
            -fcf-protection

# Linker hardening flags
LDFLAGS = -Wl,-z,relro \
          -Wl,-z,now \
          -Wl,-z,noexecstack \
          -Wl,-z,separate-code \
          -pie \
          -flto

# Optimization
OPTFLAGS = -O3 -march=native -flto

# Extract includes and convert them to -isystem
SYSTEM_INCLUDES = $(shell pkg-config --cflags libcjson libcurl | sed 's/-I/-isystem /g')

# Combined all flags
ALL_CFLAGS = $(CFLAGS) $(HARDENING) $(OPTFLAGS) $(SYSTEM_INCLUDES) -Isrc
LDLIBS = $(shell pkg-config --libs libcjson libcurl) -lm

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

TARGET = $(BIN_DIR)/exchange
SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/include/*.c)
HDRS = $(wildcard $(SRC_DIR)/*.h)

# Correct OBJS generation to avoid duplicate objects
OBJS = $(OBJ_DIR)/api.o $(OBJ_DIR)/convert.o $(OBJ_DIR)/currencies.o $(OBJ_DIR)/main.o $(OBJ_DIR)/include/set.o

.PHONY: all clean install run format lint asan

all: clean format $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Build with AddressSanitizer
asan: clean
	$(MAKE) ALL_CFLAGS="$(ALL_CFLAGS) -fsanitize=address -g" LDFLAGS="$(LDFLAGS) -fsanitize=address" $(TARGET)

format:
	clang-format -style=file:./.clang-format -i $(SRCS) $(HDRS)
	mbake format --config ./.bake.toml Makefile

# Minimal linter to avoid blocking progress
CLANG_TIDY_CHECKS = -checks=-*,bugprone-*,clang-analyzer-*
CLANG_TIDY_FLAGS = -std=c99 -pedantic -Wall -Wextra -Isrc -Isrc/include

lint:
	clang-tidy $(CLANG_TIDY_CHECKS) $(SRCS) -- $(CLANG_TIDY_FLAGS)

fix:
	clang-tidy --fix $(CLANG_TIDY_CHECKS) $(SRCS) -- $(CLANG_TIDY_FLAGS)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(ALL_CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) -c $< -o $@

$(OBJ_DIR)/include/%.o: $(SRC_DIR)/include/%.c | $(OBJ_DIR)/include
	mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR) $(OBJ_DIR)/include:
	mkdir -p $@

install: $(TARGET)
	mkdir -p $(HOME)/.local/bin
	install -m 755 $(TARGET) $(HOME)/.local/bin/

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
