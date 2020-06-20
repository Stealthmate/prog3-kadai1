# Makefile
# Date: 2020-06
# Author: 18TI018 Haralanov Valeri

CC:=gcc

# We add src/ to the include path because it contains submodules
INCLUDE_DIR:=src

CFLAGS:=-fPIC -z defs -g -I$(INCLUDE_DIR)


SOURCES:=$(shell find src -name "*.c")
# Generate a list of .o files from all the sources
OBJECTS:=$(patsubst src/%.c,build/objs/%.o,$(SOURCES))

# Since make does not apply pattern matching recursively, we need to define the targets beforehand
# Source:
# https://stackoverflow.com/questions/59048388/makefile-ignores-pattern-rule-with-subdirectory-in-pattern
$(OBJECTS): build/objs/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# All shared object submodules are made the same way, so we define a function
# in order to keep things simple
define make-shared =
mkdir -p $(@D)
$(CC) $(CFLAGS) $^ -shared -o $@ -lpthread
endef

# Explicitly define build rules for every shared object submodule
build/collections.so: $(filter build/objs/collections/%,$(OBJECTS))
	$(make-shared)
build/protocol.so: $(filter build/objs/protocol/%,$(OBJECTS))
	$(make-shared)
# build/interactive_log.so: $(filter build/objs/collections/%,$(OBJECTS))
# 	$(make-shared)

# All executables are also build in the same way, so we define the recipe as a function again
define make-exec =
mkdir -p $(@D)
$(CC) $(CFLAGS) $(filter %.o,$^) -o $@ -lpthread -Lbuild $(patsubst build/%.so,-l:%.so,$(filter %.so,$^))
endef

# Explicitly define build rules for every executable
build/server: $(filter build/objs/server/%,$(OBJECTS)) build/protocol.so build/collections.so
	$(make-exec)
build/client: $(filter build/objs/client/%,$(OBJECTS)) build/protocol.so build/collections.so
	$(make-exec)

all-shared: build/collections.so build/protocol.so

all-exec: build/server build/client

all: all-shared all-exec

clean:
	rm -rf build


# tests: build/tests/test_stack build/tests/test_linked_list build/tests/test_set
# build/tests/test_stack: build/objs/tests/test_stack.o build/objs/collections/stack.o build/objs/util.o
# 	mkdir -p $(@D)
# 	$(CC) $^ -o $@ -lpthread

# build/tests/test_linked_list: build/objs/tests/test_linked_list.o build/objs/collections/linked_list.o
# 	mkdir -p $(@D)
# 	$(CC) $^ -o $@ -lpthread

# build/tests/test_set: build/objs/tests/test_set.o build/objs/collections/set.o build/objs/collections/linked_list.o
# 	mkdir -p $(@D)
# 	$(CC) $^ -o $@ -lpthread

# build/tests/test_interactive_log: build/objs/tests/test_interactive_log.o build/interactive_log.so
# 	mkdir -p $(@D)
# 	$(CC) $(filter %.o,$^) -o $@ -lpthread -Lbuild/tests $(patsubst build/%.so,-l:../%.so,$(filter %.so,$^))

# VALGRIND=valgrind --leak-check=full --error-exitcode=1
# runTests: build/tests/test_linked_list build/tests/test_set
# 	$(VALGRIND) build/tests/test_linked_list
# 	$(VALGRIND) build/tests/test_set
