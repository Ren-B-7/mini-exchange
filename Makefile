CC = gcc
# Strict compilation flags
CFLAGS = -std=c99 \
         -pedantic \
         -pedantic-errors \
         -Wall \
         -Wextra \
         -Werror \
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

# GTK flags
# GTK_FLAGS = `pkg-config --cflags --libs gtk+-3.0`

# Extract includes and convert them to -isystem
SYSTEM_INCLUDES = $(shell pkg-config --cflags libcjson libcurl | sed 's/-I/-isystem /g')

# Combined all flags
ALL_CFLAGS = $(CFLAGS) $(HARDENING) $(OPTFLAGS) $(SYSTEM_INCLUDES)
LDLIBS = $(shell pkg-config --libs libcjson libcurl) -lm

TARGET = exchange
OBJS = main.o api.o convert.o currencies.o
SRCS = main.c api.c convert.c currencies.c
HDRS = api.h convert.h currencies.h

.PHONY: all clean install run format lint asan

all: clean format lint $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Build with AddressSanitizer
asan: clean
	$(MAKE) ALL_CFLAGS="$(ALL_CFLAGS) -fsanitize=address -g" LDFLAGS="$(LDFLAGS) -fsanitize=address" $(TARGET)
	@echo "Build complete. Run './exchange' with 'ASAN_OPTIONS=detect_leaks=1' to check for leaks."

format:
	clang-format -style=file:./.clang-format -i $(SRCS) $(HDRS)
	mbake format --config ./.bake.toml Makefile

CLANG_TIDY_CHECKS = -checks=-bugprone-easily-swappable-parameters,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,-readability-function-cognitive-complexity
CLANG_TIDY_FLAGS = -std=c99 -pedantic -Wall -Wextra -Werror $(SYSTEM_INCLUDES)

lint:
	clang-tidy $(CLANG_TIDY_CHECKS) $(SRCS) -- $(CLANG_TIDY_FLAGS)
	mbake validate --config ./.bake.toml Makefile

fix:
	clang-tidy --fix $(CLANG_TIDY_CHECKS) $(SRCS) -- $(CLANG_TIDY_FLAGS)

$(TARGET): $(OBJS)
	$(CC) $(ALL_CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

%.o: %.c
	$(CC) $(ALL_CFLAGS) -c $< -o $@

install: $(TARGET)
	mkdir -p $(HOME)/.local/bin
	install -m 755 $(TARGET) $(HOME)/.local/bin/

clean:
	rm -f $(TARGET) $(OBJS)
