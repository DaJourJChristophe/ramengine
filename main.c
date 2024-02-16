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

  observable_t *observable = NULL;
  observable = observable_new(QUEUE_CAPACITY, MAX_OBSERVERS, MAX_THREADS);

  observer1 = observer_new(observable, &notifier1);
  observer2 = observer_new(observable, &notifier2);

  observable_subscribe(observable, observer1);
  observable_subscribe(observable, observer2);

  threadpool thpool = NULL;
  thpool = thpool_init(MAX_THREADS);

  observer_t *observer = NULL;
  uint64_t i;

  for (i = 0; i < observable->count; i++)
  {
    observer = observable->observers[i];

    if (thpool_add_work(thpool, (void (*)(void *))observer->notify, observer) < 0)
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not create thread");
      exit(EXIT_FAILURE);
    }
  }

  for (i = 1; i < 6; i++)
  {
    observable_publish(observable, i);
  }

  observable_shutdown(observable);

  thpool_wait(thpool);
  thpool_destroy(thpool);

  observable_destroy(observable);

  return EXIT_FAILURE;
}
