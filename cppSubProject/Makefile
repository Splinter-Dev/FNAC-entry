WINDOWS ?= 0

CFLAGS ?= -std=c++17 -O0 -Wall -Wextra -pedantic

ifeq ($(WINDOWS),1)
    CC = x86_64-w64-mingw32-g++
    TARGET = ./program.exe
    CFLAGS += -DPLATFORM_WINDOWS -DPLATFORM_DESKTOP
    INCDIR = ./inc ./windows/inc
    LIBS = ./windows/lib/libraylib.a -lopengl32 -lgdi32 -lwinmm -lshell32 -luser32 -lkernel32
    LDFLAGS = -static -static-libgcc -static-libstdc++ -Wl,-subsystem,console
else
    CC = g++
    TARGET = ./program
    LIBS = -lraylib -lGL -lm -ldl -lrt
	INCDIR = ./inc
endif

SRC_DIR = ./src
OBJ_DIR = ./obj

# main sources
SRCS := $(shell find $(SRC_DIR) -name '*.cc')
OBJS := $(patsubst $(SRC_DIR)/%.cc,$(OBJ_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(foreach dir,$(INCDIR),-I$(dir)) $(CFLAGS)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)	

.PHONY: clean
