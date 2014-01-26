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

typedef struct
{
  Subscription *first;
  Subscription *last;
} Subscriptions;

void
subscriptions_new(Subscriptions *subscriptions);

void
subscriptions_destroy(Subscriptions *subscriptions);

bool
subscribe(Subscriptions *subscriptions, char *topic,
    void *subscriber);
bool
unsubscribe(Subscriptions *subscriptions, char *topic,
    void *subscriber);
bool
subscribed_to(Subscriptions *subscriptions, char *topic,
    void *subscriber);
Subscription *
subscription_get(Subscriptions *subscribers, char *topic);
