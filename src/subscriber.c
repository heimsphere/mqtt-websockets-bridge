#include "bridge.h"

Subscription *
subscription_new(Subscriptions *subscriptions, const char *topic)
{
	Subscription *new_subscription = calloc(1, sizeof(Subscription));

	new_subscription->prev = NULL;
	new_subscription->next = NULL;
	new_subscription->topic = strdup(topic);
	new_subscription->count_subscribed = 0;
	memset(new_subscription->subscribers, 0, TOPIC_MAX_SUBSCRIBERS);

	if (subscriptions->last)
	{
		new_subscription->prev = subscriptions->last;
		subscriptions->last->next = new_subscription;
		subscriptions->last = new_subscription;
	}
	else
	{
		subscriptions->first = new_subscription;
		subscriptions->last = new_subscription;
	}
	return new_subscription;
}

void
subscription_destroy(Subscription *sub)
{
	free(sub->topic);
	free(sub);
}

Subscription *
subscription_get(Subscriptions *subscribers, const char *topic)
{
	if (subscribers->first)
	{
		Subscription *sub;
		for (sub = subscribers->first; sub != NULL; sub = sub->next)
		{
			bool result;
			mosquitto_topic_matches_sub(sub->topic, topic, &result);
			if (result)
				return sub;
		}
	}
	return NULL;
}

/*
 * @return true if subscriber was added
 */
bool
subscription_add_subscriber(Subscription *sub, void *subscriber)
{
	assert(sub);

	if (sub->count_subscribed == TOPIC_MAX_SUBSCRIBERS)
	{
		llog(LOG_ERR, "Maximum subscribers %d reached for topic: %s\n",
		     TOPIC_MAX_SUBSCRIBERS, sub->topic);
		return false;
	}
	else
	{
		sub->subscribers[sub->count_subscribed] = subscriber;
		sub->count_subscribed++;
	}
	return true;
}

/*
 * @return true if subscriber was removed, false else
 */
bool
subscription_remove_subscriber(Subscriptions *subscriptions, Subscription *sub,
			       void *subscriber)
{
	assert(sub);

	int i;
	int idx_last = sub->count_subscribed - 1;
	for (i = 0; i <= idx_last; i++)
	{
		if (sub->subscribers[i] == subscriber)
		{
			/* the last one fills the hole */
			sub->subscribers[i] = sub->subscribers[idx_last];
			sub->subscribers[idx_last] = NULL;
			sub->count_subscribed = idx_last;

			if (subscriptions->unsubscribe_cb)
				subscriptions->unsubscribe_cb(subscriptions, sub, subscriber);

			return true;
		}
	}
	return false;
}

void
subscriptions_new(Subscriptions *subscriptions)
{
	subscriptions->first = NULL;
	subscriptions->last = NULL;
	subscriptions->unsubscribe_cb = NULL;
}

void
subscriptions_destroy(Subscriptions *subscriptions)
{
	//
}

/*
 * @return true if given subscriber subscribed to given description
 */
bool
subscribed_to(Subscriptions *subscriptions, const char *topic, void *subscriber)
{
	Subscription *sub = subscription_get(subscriptions, topic);

	if (sub)
	{
		int i;
		for (i = 0; i < sub->count_subscribed; i++)
			if (sub->subscribers[i] == subscriber)
				return true;
	}
	return false;
}

bool
subscribe(Subscriptions *subscriptions, const char *topic, void *subscriber)
{
	Subscription *sub = subscription_get(subscriptions, topic);

	if (!sub)
		sub = subscription_new(subscriptions, topic);

	return subscription_add_subscriber(sub, subscriber);
}

bool
unsubscribe(Subscriptions *subscriptions, const char *topic, void *subscriber)
{
	Subscription *sub = subscription_get(subscriptions, topic);

	if (sub)
		return subscription_remove_subscriber(subscriptions, sub, subscriber);
	return false;
}

void
unsubscribe_all(Subscriptions *subscriptions, void *subscriber)
{
	Subscription *sub;

	for (sub = subscriptions->first; sub != NULL; sub = sub->next)
		subscription_remove_subscriber(subscriptions, sub, subscriber);
}
