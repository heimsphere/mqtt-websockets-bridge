#include <stdbool.h>

#define TOPIC_MAX_SUBSCRIBERS 100

typedef struct subscription_t
{
  char *topic;
  void *subscribers[TOPIC_MAX_SUBSCRIBERS];
  int count_subscribed;
  struct subscription_t *next;
  struct subscription_t *prev;
} Subscription;

typedef struct subscriptions_t
{
  Subscription *first;
  Subscription *last;
  void
  (*unsubscribe_cb)(struct subscriptions_t *subscriptions,
      Subscription *subscription, void *subscriber);
} Subscriptions;

void
subscriptions_new(Subscriptions *subscriptions);

void
subscriptions_destroy(Subscriptions *subscriptions);

bool
subscribe(Subscriptions *subscriptions, const char *topic, void *subscriber);

bool
unsubscribe(Subscriptions *subscriptions, const char *topic, void *subscriber);

bool
subscribed_to(Subscriptions *subscriptions, const char *topic, void *subscriber);

Subscription *
subscription_get(Subscriptions *subscribers, const char *topic);

void
unsubscribe_all(Subscriptions *subscriptions, void *subscriber);
