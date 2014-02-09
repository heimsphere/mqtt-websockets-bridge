#include "unity_fixture.h" 
#include <stdio.h>
#include "../src/message.h"

TEST_GROUP(message);

TEST_SETUP(message)
{

}

TEST_TEAR_DOWN(message)
{

}

TEST(message, New)
{
  Message msg;
  message_new(&msg, PUBLISH, "/hello/world", "hello world");

  TEST_ASSERT_EQUAL(msg.method, PUBLISH);
  TEST_ASSERT_EQUAL_STRING(msg.topic, "/hello/world");
  TEST_ASSERT_EQUAL_STRING(msg.data, "hello world");
}

TEST(message, NewNullData)
{
  Message msg;
  message_new(&msg, SUBSCRIBE, "/hello/world", NULL);

  TEST_ASSERT_EQUAL(msg.method, SUBSCRIBE);
  TEST_ASSERT_EQUAL_STRING(msg.topic, "/hello/world");
  TEST_ASSERT_NULL(msg.data);
}

TEST(message, Free)
{
  Message msg;
  message_new(&msg, PUBLISH, "/hello/world", "hello world");

  message_free(&msg);
  TEST_ASSERT_NULL(msg.topic);
  TEST_ASSERT_NULL(msg.data);
  message_free(&msg);
}

TEST(message, ParseSerializeSubscribe)
{
  Message msg;
  Message msg_deserialized;
  int res;

  message_new(&msg, SUBSCRIBE, "/hello/world", NULL);
  message_serialize_request(&msg);
  res = message_parse(&msg_deserialized, msg.serialized);

  TEST_ASSERT_TRUE(res > 0);
  TEST_ASSERT_EQUAL_STRING(msg.topic, msg_deserialized.topic);
  TEST_ASSERT_NULL(msg_deserialized.data);
  TEST_ASSERT_EQUAL(msg.method, msg_deserialized.method);

  message_free(&msg);
  message_free(&msg_deserialized);
}

TEST(message, ParseSerializeUnsubscribe)
{
  Message msg;
  Message msg_deserialized;
  int res;

  message_new(&msg, UNSUBSCRIBE, "/hello/world", NULL);
  message_serialize_request(&msg);
  res = message_parse(&msg_deserialized, msg.serialized);

  TEST_ASSERT_TRUE(res > 0);
  TEST_ASSERT_EQUAL_STRING(msg.topic, msg_deserialized.topic);
  TEST_ASSERT_NULL(msg_deserialized.data);
  TEST_ASSERT_EQUAL(msg.method, msg_deserialized.method);

  message_free(&msg);
  message_free(&msg_deserialized);
}

TEST(message, ParseSerializePublish)
{
  Message msg;
  Message msg_deserialized;
  int res;

  message_new(&msg, PUBLISH, "/hello/world", "hello world");
  message_serialize_request(&msg);
  res = message_parse(&msg_deserialized, msg.serialized);

  TEST_ASSERT_TRUE(res > 0);
  TEST_ASSERT_EQUAL_STRING(msg.topic, msg_deserialized.topic);
  TEST_ASSERT_EQUAL_STRING(msg.data, msg_deserialized.data);
  TEST_ASSERT_EQUAL(msg.method, msg_deserialized.method);

  message_free(&msg);
  message_free(&msg_deserialized);
}
 
TEST_GROUP_RUNNER(message)
{ 
    RUN_TEST_CASE(message, New);
    RUN_TEST_CASE(message, NewNullData);
    RUN_TEST_CASE(message, Free);
    RUN_TEST_CASE(message, ParseSerializeSubscribe);
    RUN_TEST_CASE(message, ParseSerializeUnsubscribe);
    RUN_TEST_CASE(message, ParseSerializePublish);
}
