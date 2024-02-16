#include "broker.h"

#include <C-Thread-Pool/thpool.h>
#include <hyperfunnel/observable.h>
#include <hyperfunnel/observer.h>

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

broker_t *broker_new(const size_t cap, const size_t max_observers, const size_t max_threads)
{
  broker_t *self = NULL;
  self = (broker_t *)calloc(1, sizeof(*self));
  self->observable = observable_new(cap, max_observers, 0);
  self->thpool = thpool_init(max_threads);
  return self;
}

void broker_destroy(broker_t *self)
{
  if (self != NULL)
  {
    thpool_destroy(self->thpool);

    observable_destroy(self->observable);

    free(self);
    self = NULL;
  }
}

void broker_shutdown(broker_t *self)
{
  if (self != NULL)
  {
    observable_shutdown(self->observable);
    thpool_wait(self->thpool);
  }
}

void broker_start(broker_t *self)
{
  observer_t *observer = NULL;
  uint64_t i;

  for (i = 0; i < self->observable->count; i++)
  {
    observer = self->observable->observers[i];

    if (thpool_add_work(self->thpool, (void (*)(void *))observer->notify, observer) < 0)
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not create thread");
      exit(EXIT_FAILURE);
    }
  }
}

bool broker_publish(broker_t *self, const int item)
{
  return observable_publish(self->observable, item);
}

bool broker_subscribe(broker_t *self, observer_t *observer)
{
  return observable_subscribe(self->observable, observer);
}
