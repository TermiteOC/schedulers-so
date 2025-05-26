#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "timer.h"
#include "task.h"
#include "list.h"
#include "CPU.h"
#include "schedule_edf.h"

#define MAX_TASKS 100

Task *tasks[MAX_TASKS];
int num_tasks = 0;

void add(char *name, int priority, int burst, int deadline) {
    if (num_tasks >= MAX_TASKS) {
        printf("Erro: número máximo de tarefas excedido!\n");
        return;
    }

    Task *t = malloc(sizeof(Task));
    t->name = strdup(name);
    t->priority = priority;
    t->burst = burst;
    t->deadline = deadline;

    tasks[num_tasks++] = t;
}

// encontra índice com menor deadline
int find_next_task() {
    int min_index = -1;
    int min_deadline = __INT_MAX__;

    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i]->burst > 0 && tasks[i]->deadline < min_deadline) {
            min_deadline = tasks[i]->deadline;
            min_index = i;
        }
    }

    return min_index;
}

void schedule() {
    start_timer();

    while (1) {
        int idx = find_next_task();
        if (idx == -1) break;

        Task *t = tasks[idx];
        int slice = (t->burst > QUANTUM) ? QUANTUM : t->burst;

        run(t, slice);
        t->burst -= slice;

        usleep(slice * 10);
    }

    stop_timer();

    for (int i = 0; i < num_tasks; i++) {
        free(tasks[i]->name);
        free(tasks[i]);
    }
    num_tasks = 0;
}
