/*
 * subscriber.c
 *
 *  Created on: 24.01.2014
 *      Author: ruben
 */
#include "bridge.h"

Subscription *
subscription_new(Subscriptions *subscribers, char *topic)
{
  Subscription *new = calloc(1, sizeof(Subscription));

  if (subscribers->last)
    {
      new->prev = subscribers->last;
      subscribers->last->next = new;
      subscribers->last = new;
    }
  else
    {
      subscribers->first = new;
      subscribers->last = new;
      new->prev = NULL;
      new->next = NULL;
      new->topic = strdup(topic);
      new->count_subscribed = 0;
      memset(new->subscribers, 0, TOPIC_MAX_SUBSCRIBERS);
    }
  return new;
}

void
subscription_destroy(Subscriptions *subscribers, Subscription *sub)
{
  assert(false);
}

Subscription *
subscription_get(Subscriptions *subscribers, char *topic)
{
  if (subscribers->first)
    {
      Subscription *sub;
      for (sub = subscribers->first; sub->next; sub = sub->next)
        {
          bool result;
          mosquitto_topic_matches_sub(topic, sub->topic, &result);
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
subscription_remove_subscriber(Subscription *sub,
    void *subscriber, bool remove_if_empty)
{
  assert(sub);
  int i;
  int idx_last = sub->count_subscribed - 1;
  for (i = 0; i <= idx_last; i++)
    {
      if (sub->subscribers[i] == subscriber)
        {
          // last one fills the hole
          sub->subscribers[i] = sub->subscribers[idx_last];
          sub->subscribers[idx_last] = NULL;
          sub->count_subscribed = idx_last;
          return true;
        }
    }
  return false;
}

/*
 * @return true if given subscriber subscribed to given description
 */
bool
subscribed_to(Subscriptions *subscriptions, char *topic,
    void *subscriber)
{
  Subscription *sub = subscription_get(subscriptions, topic);
  if (sub)
    {
      int i;
      for (i = 0; sub->count_subscribed - 1; i++)
        {
          if (sub->subscribers[i] == subscriber)
            return true;
        }
    }
  return false;
}

bool
subscribe(Subscriptions *subscriptions, char *topic,
    void *subscriber)
{
  Subscription *sub = subscription_get(subscriptions, topic);
  if (!sub)
    sub = subscription_new(subscriptions, topic);

  return subscription_add_subscriber(sub, subscriber);
}

bool
unsubscribe(Subscriptions *subscriptions, char *topic,
    void *subscriber)
{
  Subscription *sub = subscription_get(subscriptions, topic);
  if (!sub)
    return false;
  else
    return subscription_remove_subscriber(sub, subscriber, false);
}
