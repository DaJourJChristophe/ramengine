#include "broker.h"

#include <C-Thread-Pool/thpool.h>

#include <hyperfunnel/observable.h>
#include <hyperfunnel/observer.h>

#include <inttypes.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void *notifier1(void *args)
{
  observer_t *observer = (observer_t *)args;
  int *item = NULL;

  while (false == atomic_load(&observer->observable->done) ||
         false == ts_queue_empty(observer->observable->queue))
  {
    observable_select(observer->observable);

    while (NULL != (item = ts_queue_dequeue(observer->observable->queue)))
    {
      printf("%d\n", *item);
    }

    observable_clear(observer->observable);
  }

  return NULL;
}

void *notifier2(void *args)
{
  observer_t *observer = (observer_t *)args;
  int *item = NULL;

  while (false == atomic_load(&observer->observable->done) ||
         false == ts_queue_empty(observer->observable->queue))
  {
    observable_select(observer->observable);

    while (NULL != (item = ts_queue_dequeue(observer->observable->queue)))
    {
      printf("%d\n", *item);
    }

    observable_clear(observer->observable);
  }

  return NULL;
}

#define QUEUE_CAPACITY 10
#define MAX_OBSERVERS   5
#define MAX_THREADS     2

int main(void)
{
  observer_t *observer1 = NULL;
  observer_t *observer2 = NULL;

  broker_t *broker = NULL;
  broker = broker_new(QUEUE_CAPACITY, MAX_OBSERVERS, MAX_THREADS);

  observer1 = observer_new(broker->observable, &notifier1);
  observer2 = observer_new(broker->observable, &notifier2);

  broker_subscribe(broker, observer1);
  broker_subscribe(broker, observer2);

  broker_start(broker);

  int i;

  for (i = 1; i < 6; i++)
  {
    broker_publish(broker, i);
  }

  broker_shutdown(broker);
  broker_destroy(broker);

  return EXIT_FAILURE;
}
