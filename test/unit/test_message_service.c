#include "unity_fixture.h" 
#include <stdio.h>

TEST_GROUP(message_service);

TEST_SETUP(message_service)
{

}

TEST_TEAR_DOWN(message_service)
{

}

TEST(message_service, TestCase1)
{
//    TEST_ASSERT_TRUE(0);
}
 
TEST_GROUP_RUNNER(message_service)
{ 
    RUN_TEST_CASE(message_service, TestCase1);
}
