TESTS=$(shell ls test_*.c | cut -d'.' -f1 | sed 's/^test_//')
OUT=.run_tests
TEMPLATE=bin/group.c.template
UNITY_SRC=https://codeload.github.com/ThrowTheSwitch/Unity/zip/master
SRC=variable_scope.c ../src/subscriber.c ../src/llog.c

LIBWS_PATH=`brew --prefix libwebsockets`
LIBEV_PATH=`brew --prefix libev`
MOSQUITTO_PATH=`brew --prefix mosquitto`
JANSSON_PATH=`brew --prefix jansson`

CFLAGS=-Wall -Wp -w -g -dwarf-2 -I./unity -I../src -DTEST
# Specify your preferred editor which should open a new test file/group.
# Set to `true` to do nothing.
OPEN_IN_EDITOR=true

# Execute test runner after compilation
default: compile
	./$(OUT)

clean:
	rm -f $(OUT) $(OUT).c

# Downloads and extracts unity sources into ./unity
bootstrap:
	curl $(UNITY_SRC) > .unity.zip && \
	unzip .unity.zip Unity-master/src/* Unity-master/extras/fixture/src/* -d unity &&\
	mv unity/Unity-master/src/* unity/Unity-master/extras/fixture/src/* unity && \
	rm -rf unity/Unity-master .unity.zip

# Creates the test runner source file which calls
# all TEST_GROUPs defined by $(TEST)
assemble:
	./bin/assemble_groups.sh $(TESTS) > $(OUT).c

# Compiles the test runner created by `assemble`
compile: assemble
	gcc $(CFLAGS) \
	unity/unity.c unity/unity_fixture.c $(SRC) -o $(OUT) \
	$(foreach var,$(TESTS), test_$(var).c) $(OUT).c \
	-I$(LIBWS_PATH)/include/ -L$(LIBWS_PATH)/lib -lwebsockets \
	-I$(MOSQUITTO_PATH)/include/ -L$(MOSQUITTO_PATH)/lib -lmosquitto \
	-I$(JANSSON_PATH)/include/ -L$(JANSSON_PATH)/lib -ljansson

# Creates a new TEST_GROUP `test_<name>.c` from $(TEMPLATE)
# @param[name] : the TEST_GROUP name
new:
	test -n "$(name)" && ! test -f "test_$(name).c" && \
	cat $(TEMPLATE) | sed "s/\<GROUP\>/$(name)/g" > test_$(name).c && \
	$(OPEN_IN_EDITOR) test_$(name).c \



