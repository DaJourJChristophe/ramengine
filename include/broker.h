#ifndef RAMENGINE__BROKER_H
#define RAMENGINE__BROKER_H

#include <C-Thread-Pool/thpool.h>
#include <hyperfunnel/observable.h>

struct broker
{
  observable_t *observable;
  threadpool thpool;
};

typedef struct broker broker_t;

broker_t *broker_new(const size_t cap, const size_t max_observers, const size_t max_threads);

void broker_destroy(broker_t *self);

void broker_shutdown(broker_t *self);

void broker_start(broker_t *self);

bool broker_publish(broker_t *self, const int item);

bool broker_subscribe(broker_t *self, observer_t *observer);

#endif/*RAMENGINE__BROKER_H*/
