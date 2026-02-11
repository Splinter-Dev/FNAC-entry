WINDOWS ?= 0

CFLAGS ?= -std=c++17 -O0 -Wall -Wextra -Wno-unused-parameter -ffast-math

SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
LIB_DIR = ./lib
APP_DIR = ./apps

ifeq ($(WINDOWS),1)
    CC = x86_64-w64-mingw32-g++
    TARGET = $(BIN_DIR)/program.exe
    CFLAGS += -DPLATFORM_WINDOWS -DPLATFORM_DESKTOP -Wno-missing-field-initializers
    INCDIR = ./inc ./windows/inc
    LIBS = ./windows/lib/libraylib.a -lopengl32 -lgdi32 -lwinmm -lshell32 -luser32 -lkernel32
    LDFLAGS = -static -static-libgcc -static-libstdc++ -Wl,-subsystem,console
	LIB_TARGET = $(LIB_DIR)/program.lib
else
    CC = g++
    TARGET = $(BIN_DIR)/program
    LIBS = -lraylib -lGL -lm -ldl -lrt
	INCDIR = ./inc
	LIB_TARGET = $(LIB_DIR)/program.a
endif

# main sources
SRCS := $(shell find $(SRC_DIR) -name '*.cc')
OBJS := $(patsubst $(SRC_DIR)/%.cc,$(OBJ_DIR)/%.o,$(SRCS))
APPS := $(shell find $(APP_DIR) -name '*.cc')
# Turn apps/foo.cc into bin/foo
BINS := $(patsubst $(APP_DIR)/%.cc,$(BIN_DIR)/%,$(APPS))

all: $(BINS)

$(LIB_TARGET): $(OBJS)
	mkdir -p $(LIB_DIR)
	ar rcs $@ $^

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build rule
$(BIN_DIR)/%: $(APP_DIR)/%.cc | $(BIN_DIR) $(LIB_TARGET)
	$(CC) $(CFLAGS) $(foreach dir,$(INCDIR),-I$(dir)) $< -o $@ $(LIB_TARGET) $(LDFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(foreach dir,$(INCDIR),-I$(dir)) $(CFLAGS)

$(TARGET): $(OBJS) $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

static: $(LIB_TARGET)

.PHONY: clean
