#include <pthread.h>
#include <unistd.h>
#include "timer.h"

int time_elapsed = 0;
pthread_t timer;

void *timer_thread(void *arg) {
    while (1) {
        sleep(1);
        time_elapsed++;
    }
    return NULL;
}

void start_timer() {
    time_elapsed = 0;
    pthread_create(&timer, NULL, timer_thread, NULL);
}

void stop_timer() {
    pthread_cancel(timer);
}