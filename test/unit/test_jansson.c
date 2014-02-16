#include "unity_fixture.h"
#include <jansson.h>
#include <stdio.h>

TEST_GROUP(jansson);

TEST_SETUP(jansson)
{

}

TEST_TEAR_DOWN(jansson)
{

}

TEST(jansson, TestCase1)
{
	json_t *root;
	json_error_t error;

	root = json_loads("{}", 0, &error);
	TEST_ASSERT_TRUE(json_is_object(root));
	TEST_ASSERT_EQUAL(JSON_OBJECT, json_typeof(root));
}

TEST_GROUP_RUNNER(jansson)
{
	RUN_TEST_CASE(jansson, TestCase1);
}
