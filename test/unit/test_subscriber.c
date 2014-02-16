#include "unity_fixture.h"
#include <stdio.h>
#include "subscriber.h"

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

TEST(subscriber, SubscribeMultipleClients)
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

TEST(subscriber, SubscribeMultipleTopics)
{
	Subscriptions s;

	subscriptions_new(&s);
	const char *subscriber1 = "subscriber 1";
	const char *topic1 = "/foo/bar";
	const char *topic2 = "/foo/baz";
	subscribe(&s, topic1, &subscriber1);
	subscribe(&s, topic2, &subscriber1);

	TEST_ASSERT_TRUE(subscribed_to(&s, topic1, &subscriber1));
	TEST_ASSERT_TRUE(subscribed_to(&s, topic2, &subscriber1));
}

TEST(subscriber, NotSubscribed)
{
	Subscriptions s;

	subscriptions_new(&s);
	const char *subscriber1 = "subscriber 1";
	const char *topic = "/foo/bar";

	TEST_ASSERT_FALSE(subscribed_to(&s, topic, &subscriber1));
}

TEST(subscriber, UnsubscribeAll)
{
	Subscriptions s;

	subscriptions_new(&s);
	const char *subscriber = "subscriber";
	const char *topic = "/foo/bar";
	const char *topic1 = "/foo/bar1";
	const char *topic2 = "/foo/bar2";

	subscribe(&s, topic, &subscriber);
	subscribe(&s, topic1, &subscriber);
	subscribe(&s, topic2, &subscriber);

	TEST_ASSERT_TRUE(subscribed_to(&s, topic, &subscriber));
	TEST_ASSERT_TRUE(subscribed_to(&s, topic1, &subscriber));
	TEST_ASSERT_TRUE(subscribed_to(&s, topic2, &subscriber));

	unsubscribe_all(&s, &subscriber);

	TEST_ASSERT_FALSE(subscribed_to(&s, topic, &subscriber));
	TEST_ASSERT_FALSE(subscribed_to(&s, topic1, &subscriber));
	TEST_ASSERT_FALSE(subscribed_to(&s, topic2, &subscriber));
}

TEST(subscriber, SubscriptionCount)
{
	Subscriptions s;

	subscriptions_new(&s);
	const char *subscriber = "subscriber 1";
	const char *topic = "/foo/bar";

	subscribe(&s, topic, &subscriber);
	TEST_ASSERT_EQUAL(subscription_get(&s, topic)->count_subscribed, 1);
	unsubscribe_all(&s, &subscriber);
	TEST_ASSERT_EQUAL(subscription_get(&s, topic)->count_subscribed, 0);
}

TEST_GROUP_RUNNER(subscriber)
{
	RUN_TEST_CASE(subscriber, Subscribe);
	RUN_TEST_CASE(subscriber, Unsubscribe);
	RUN_TEST_CASE(subscriber, SubscribeMultipleClients);
	RUN_TEST_CASE(subscriber, SubscribeMultipleTopics);
	RUN_TEST_CASE(subscriber, NotSubscribed);
	RUN_TEST_CASE(subscriber, UnsubscribeAll);
	RUN_TEST_CASE(subscriber, SubscriptionCount);
}
