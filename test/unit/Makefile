# see http://www.gnu.org/software/make/manual/html_node/Include.html
-include Make.conf

# Specify your preferred editor which should open a new test file/group.
# Set to `true` to do nothing.
EDITOR ?= true

SRC_DIR ?= ../src
TEST_GROUP_TEMPLATE ?= bin/group.c.template
CFLAGS += -Wall -Wp -w -g -I./unity -I$(SRC_DIR) -DTEST
UNITY_URL ?= https://codeload.github.com/ThrowTheSwitch/Unity/zip/master
UNITY_SRC ?= unity/unity.c unity/unity_fixture.c
TEST_RUNNER ?= .run_tests
TESTS ?= $(patsubst test_%.c, %, $(shell ls test_*.c 2>/dev/null))

TEST_SOURCE = $(patsubst %, test_%.c, $(TESTS))
SRC_OBJS=$(patsubst %.c,$(SRC_DIR)/%.o,$(SRC))
TEST_OBJS=$(patsubst %,test_%.o,$(TESTS))
UNITY_OBJS=$(patsubst %.c,%.o,$(UNITY_SRC))
OBJS=$(UNITY_OBJS) $(SRC_OBJS) $(TEST_OBJS) $(TEST_RUNNER).o

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -gdwarf-2 -g -O0
endif

default: run

# Execute test runner after compilation
run: $(TEST_RUNNER)
	./$(TEST_RUNNER)

# Compiles the test runner
$(TEST_RUNNER): $(OBJS)

# Creates the test runner source
.PHONY: $(TEST_RUNNER).c
$(TEST_RUNNER).c: $(TEST_SOURCE)
	./bin/assemble_groups.sh $(TESTS) > $(TEST_RUNNER).c
	
.PHONY: clean
clean:
	rm -f $(OBJS) $(TEST_RUNNER).c

# Downloads and extracts unity sources into ./unity
.PHONY: bootstrap
bootstrap:
	curl $(UNITY_URL) > .unity.zip && \
	unzip .unity.zip Unity-master/src/* Unity-master/extras/fixture/src/* -d unity &&\
	mv unity/Unity-master/src/* unity/Unity-master/extras/fixture/src/* unity && \
	rm -rf unity/Unity-master .unity.zip

# Replace Makefile with latest version
.PHONY: update
update:
	cp Makefile .Makefile~$(shell date +%Y-%m-%d-%H.%M.%S) && \
	curl https://raw2.github.com/Drachenfels-GmbH/unity-test-seed/master/Makefile > Makefile 

# Creates a new TEST_GROUP `test_<name>.c` from $(TEST_GROUP_TEMPLATE)
# @param[name] : the TEST_GROUP name
.PHONY: new
new:
	test -n "$(name)" && ! test -f "test_$(name).c" && \
	cat $(TEST_GROUP_TEMPLATE) | sed "s/\<GROUP\>/$(name)/g" > test_$(name).c && \
	$(EDITOR) test_$(name).c
