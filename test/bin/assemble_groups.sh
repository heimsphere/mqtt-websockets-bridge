#!/bin/sh

cat <<-EOF
#include "unity_fixture.h"
#include <stdio.h>

static void RunAllTests(void)
{
EOF

for group in $@; do
	echo "    RUN_TEST_GROUP($group);"
done

cat <<-EOF
}

int main(int argc, char * argv[])
{
    return UnityMain(argc, argv, RunAllTests);
}
EOF

