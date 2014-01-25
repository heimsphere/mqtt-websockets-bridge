#include "unity_fixture.h"
#include <mosquitto.h>
#include <stdio.h>

TEST_GROUP(topic);

TEST_SETUP(topic)
{

}

TEST_TEAR_DOWN(topic)
{

}

assert_subscribed(const char *topic, const char *subscription) {
	bool result = false;
	TEST_ASSERT_EQUAL(MOSQ_ERR_SUCCESS,
		mosquitto_topic_matches_sub(subscription, topic, &result));
	TEST_ASSERT_TRUE(result);
}

assert_not_subscribed(const char *topic, const char *subscription) {
	bool result = true;
	TEST_ASSERT_EQUAL(MOSQ_ERR_SUCCESS,
		mosquitto_topic_matches_sub(subscription, topic, &result));
	TEST_ASSERT_FALSE(result);
}

TEST(topic, EqualsSubscription)
{
	assert_subscribed("foo", "foo");
	assert_subscribed("/foo", "/foo");
	assert_subscribed("/foo/bar", "/foo/bar");
}

TEST(topic, NotEqualsSubscription)
{
	assert_not_subscribed("foo", "/foo");
	assert_not_subscribed("/foo", "foo");
	assert_not_subscribed("/foo", "/bar");
	assert_not_subscribed("/foo/bar", "/foo");
}

TEST(topic, MatchesWildcardSubscription)
{
	assert_subscribed("/foo/bar", "/foo/+");
	assert_subscribed("/foo/bar/baz", "/foo/+/baz");
}

TEST(topic, NotMatchesWildcardSubscription)
{
	assert_not_subscribed("/foo", "/foo/+");
	assert_not_subscribed("/foo/", "/foo/+");
	assert_not_subscribed("/foo/bar/baz", "/foo/+");
}

TEST(topic, MatchesMultilevelWildcardSubscription)
{
	assert_subscribed("/foo", "/foo/#");
	assert_subscribed("/foo/", "/foo/#");
	assert_subscribed("/foo/bar", "/foo/#");
	assert_subscribed("/foo/bar/baz", "/foo/#");
}

TEST(topic, NotMatchesMultilevelWildcardSubscription)
{
	assert_not_subscribed("/bar", "/foo/#");
	assert_not_subscribed("/bar/foo", "/foo/bar/#");
}

TEST(topic, MatchesMixedWildcardSubscription)
{
	assert_subscribed("/foo/xxx/bar/blub", "/foo/+/bar/#");
	assert_subscribed("/foo/yyy/bar/", "/foo/+/bar/#");
	assert_subscribed("/foo/111/bar/baz/222/blub", "/foo/+/bar/baz/+/blub/#");
}

TEST(topic, NotMatchesMixedWildcardSubscription)
{
	assert_not_subscribed("/foo/111/bar/baz/222/", "/foo/+/bar/baz/+/blub/#");
}

TEST(topic, CaseSensitiveSubscription) {
	assert_subscribed("Foo", "Foo");
	assert_not_subscribed("foo", "Foo");
	assert_not_subscribed("Foo", "foo");
}

TEST_GROUP_RUNNER(topic)
{ 
    RUN_TEST_CASE(topic, EqualsSubscription);
    RUN_TEST_CASE(topic, NotEqualsSubscription);
    RUN_TEST_CASE(topic, MatchesWildcardSubscription);
    RUN_TEST_CASE(topic, NotMatchesWildcardSubscription);
    RUN_TEST_CASE(topic, MatchesMultilevelWildcardSubscription);
    RUN_TEST_CASE(topic, NotMatchesMultilevelWildcardSubscription);
    RUN_TEST_CASE(topic, MatchesMixedWildcardSubscription);
    RUN_TEST_CASE(topic, NotMatchesMixedWildcardSubscription);
    RUN_TEST_CASE(topic, CaseSensitiveSubscription);
}
