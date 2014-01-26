#include "unity_fixture.h" 
#include <stdio.h>
#include "../src/subscriber.h"

TEST_GROUP(subscriber);

TEST_SETUP(subscriber)
{

}

TEST_TEAR_DOWN(subscriber)
{

}

TEST(subscriber, Subscribe)
{
  Subscriptions s;
  subscriptions_new(&s);
  const char *subscriber = "subscriber 1";
  const char *topic = "/foo/bar";
  subscribe(&s, topic, &subscriber);

  TEST_ASSERT_TRUE(subscribed_to(&s, topic, &subscriber));
}

TEST(subscriber, Unsubscribe)
{
  Subscriptions s;
  subscriptions_new(&s);
  const char *subscriber = "subscriber 1";
  const char *topic = "/foo/bar";
  subscribe(&s, topic, &subscriber);
  unsubscribe(&s, topic, &subscriber);
  TEST_ASSERT_FALSE(subscribed_to(&s, topic, &subscriber));
}

TEST(subscriber, SubscribeMulti)
{
  Subscriptions s;
  subscriptions_new(&s);
  const char *subscriber1 = "subscriber 1";
  const char *subscriber2 = "subscriber 2";
  const char *topic = "/foo/bar";
  subscribe(&s, topic, &subscriber1);
  subscribe(&s, topic, &subscriber2);

  TEST_ASSERT_TRUE(subscribed_to(&s, topic, &subscriber1));
  TEST_ASSERT_TRUE(subscribed_to(&s, topic, &subscriber2));
}

TEST(subscriber, NotSubscribed)
{
  Subscriptions s;
  subscriptions_new(&s);
  const char *subscriber1 = "subscriber 1";
  const char *topic = "/foo/bar";

  TEST_ASSERT_FALSE(subscribed_to(&s, topic, &subscriber1));
}
 
TEST_GROUP_RUNNER(subscriber)
{ 
    RUN_TEST_CASE(subscriber, Subscribe);
    RUN_TEST_CASE(subscriber, Unsubscribe);
    RUN_TEST_CASE(subscriber, SubscribeMulti);
    RUN_TEST_CASE(subscriber, NotSubscribed);
}
