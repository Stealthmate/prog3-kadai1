CCOMPILER:=gcc
CFLAGS:=-fPIC -z defs -g
CC:=$(CCOMPILER) $(CFLAGS)

all: build/collections.so build/server

build/objs/:
	mkdir -p build/objs

SOURCES:=$(shell find src -name "*.c")
OBJECTS:=$(patsubst src/%.c,build/objs/%.o,$(SOURCES))
$(OBJECTS): build/objs/%.o: src/%.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@

build/collections.so: $(filter build/objs/collections/%,$(OBJECTS))
	mkdir -p $(@D)
	$(CC) $^ -shared -o $@ -lpthread

build/protocol.so: $(filter build/objs/protocol/%,$(OBJECTS))
	mkdir -p $(@D)
	$(CC) $^ -shared -o $@ -lpthread

build/interactive_log.so: $(filter build/objs/interactive_log/%,$(OBJECTS))
	mkdir -p $(@D)
	$(CC) $^ -shared -o $@

build/server: $(filter build/objs/server/%,$(OBJECTS)) build/protocol.so build/collections.so
	mkdir -p $(@D)
	$(CC) $(filter %.o,$^) -o $@ -lpthread -Lbuild $(patsubst build/%.so,-l:%.so,$(filter %.so,$^))

build/client: $(filter build/objs/client/%,$(OBJECTS)) build/protocol.so build/collections.so build/interactive_log.so
	mkdir -p $(@D)
	$(CC) $(filter %.o,$^) -o $@ -lpthread -Lbuild $(patsubst build/%.so,-l:%.so,$(filter %.so,$^))

clean:
	find build -name "*.o" -delete
	rm build/collections.so
	rm build/protocol.so

tests: build/tests/test_stack build/tests/test_linked_list build/tests/test_set
build/tests/test_stack: build/objs/tests/test_stack.o build/objs/collections/stack.o build/objs/util.o
	mkdir -p $(@D)
	$(CC) $^ -o $@ -lpthread

build/tests/test_linked_list: build/objs/tests/test_linked_list.o build/objs/collections/linked_list.o
	mkdir -p $(@D)
	$(CC) $^ -o $@ -lpthread

build/tests/test_set: build/objs/tests/test_set.o build/objs/collections/set.o build/objs/collections/linked_list.o
	mkdir -p $(@D)
	$(CC) $^ -o $@ -lpthread

build/tests/test_interactive_log: build/objs/tests/test_interactive_log.o build/interactive_log.so
	mkdir -p $(@D)
	$(CC) $(filter %.o,$^) -o $@ -lpthread -Lbuild/tests $(patsubst build/%.so,-l:../%.so,$(filter %.so,$^))

VALGRIND=valgrind --leak-check=full --error-exitcode=1
runTests: build/tests/test_linked_list build/tests/test_set
	$(VALGRIND) build/tests/test_linked_list
	$(VALGRIND) build/tests/test_set
