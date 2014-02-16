#include "unity_fixture.h"
#include <stdio.h>

TEST_GROUP(variable_scope);

TEST_SETUP(variable_scope)
{

}

TEST_TEAR_DOWN(variable_scope)
{

}

TEST(variable_scope, StaticVariableInStaticFunction)
{
	TEST_ASSERT_EQUAL(getInc1(0), 0);
	TEST_ASSERT_EQUAL(getInc1(1), 1);
	TEST_ASSERT_EQUAL(getInc1(1), 2);
	TEST_ASSERT_EQUAL(getInc1(2), 4);

	TEST_ASSERT_EQUAL(getInc2(0), 0);
	TEST_ASSERT_EQUAL(getInc2(1), 1);
	TEST_ASSERT_EQUAL(getInc2(1), 2);

	TEST_ASSERT_EQUAL(getInc1(0), 4);
}

TEST(variable_scope, StaticVariableInFunction)
{
	TEST_ASSERT_EQUAL(getInc3(0), 0);
	TEST_ASSERT_EQUAL(getInc3(1), 1);
	TEST_ASSERT_EQUAL(getInc3(1), 2);
	TEST_ASSERT_EQUAL(getInc3(2), 4);

	TEST_ASSERT_EQUAL(getInc4(0), 0);
	TEST_ASSERT_EQUAL(getInc4(1), 1);
	TEST_ASSERT_EQUAL(getInc4(1), 2);

	TEST_ASSERT_EQUAL(getInc3(0), 4);
}

TEST_GROUP_RUNNER(variable_scope)
{
	RUN_TEST_CASE(variable_scope, StaticVariableInStaticFunction);
	RUN_TEST_CASE(variable_scope, StaticVariableInFunction);
}
