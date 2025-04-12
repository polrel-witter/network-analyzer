CC = gcc
CFLAGS = -Wall -Wextra -fPIC
AR = ar
ARFLAGS = rcs

# Debug flags
DEBUG_FLAGS = -g -DDEBUG
# Release flags
RELEASE_FLAGS = -O2 -DNDEBUG

# Library names
LIB_NAME = blockalloc
STATIC_LIB = lib$(LIB_NAME).a
SHARED_LIB = lib$(LIB_NAME).so

# Source files
SRCS = block-alloc.c
OBJS = $(SRCS:.c=.o)
HEADERS = block-alloc.h

# Default target
all: $(STATIC_LIB) $(SHARED_LIB)

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

# Release build
release: CFLAGS += $(RELEASE_FLAGS)
release: all

# Static library
$(STATIC_LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

# Shared library
$(SHARED_LIB): $(OBJS)
	$(CC) -shared $(OBJS) -o $@

# Object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Install (adjust PREFIX as needed)
PREFIX = /usr/local
install: all
	install -d $(PREFIX)/lib
	install -d $(PREFIX)/include
	install -m 644 $(STATIC_LIB) $(PREFIX)/lib
	install -m 755 $(SHARED_LIB) $(PREFIX)/lib
	install -m 644 $(HEADERS) $(PREFIX)/include

# Clean
clean:
	rm -f $(OBJS) $(STATIC_LIB) $(SHARED_LIB)

.PHONY: all debug release clean install
