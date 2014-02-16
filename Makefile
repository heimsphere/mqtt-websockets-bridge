SUBDIRS=test/unit src doc
CFLAGS += -g -pedantic -Wall -DEXTERNAL_POLL

test/unit: src

include make.defs