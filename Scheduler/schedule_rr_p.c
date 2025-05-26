#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "CPU.h"
#include "schedule_rr_p.h"

#define PRIORITY_LEVELS (MAX_PRIORITY + 1)

struct node *fifo[PRIORITY_LEVELS] = { NULL };

void add(char *name, int priority, int burst) {
    if (priority < MIN_PRIORITY) priority = MIN_PRIORITY;
    if (priority > MAX_PRIORITY) priority = MAX_PRIORITY;

    Task *t = malloc(sizeof(Task));
    t->name = name;
    t->priority = priority;
    t->burst = burst;

    struct node *newNode = malloc(sizeof(struct node));
    newNode->task = t;
    newNode->next = NULL;

    if (fifo[priority] == NULL) {
        fifo[priority] = newNode;
    } else {
        struct node *temp = fifo[priority];
        while (temp->next != NULL) temp = temp->next;
        temp->next = newNode;
    }
}

void schedule() {
    int tasks_remaining;

    do {
        tasks_remaining = 0;

        for (int p = MIN_PRIORITY; p <= MAX_PRIORITY; p++) {
            struct node *queue = fifo[p];
            if (queue != NULL) tasks_remaining = 1;

            while (fifo[p] != NULL) {
                Task *t = fifo[p]->task;
                int slice = (t->burst > QUANTUM) ? QUANTUM : t->burst;
                run(t, slice);
                t->burst -= slice;

                struct node *executed = fifo[p];
                fifo[p] = fifo[p]->next;
                free(executed);

                if (t->burst > 0) {
                    struct node *newNode = malloc(sizeof(struct node));
                    newNode->task = t;
                    newNode->next = NULL;

                    if (fifo[p] == NULL) {
                        fifo[p] = newNode;
                    } else {
                        struct node *temp = fifo[p];
                        while (temp->next != NULL) temp = temp->next;
                        temp->next = newNode;
                    }
                } else {
                    free(t);
                }
            }
        }

    } while (tasks_remaining);
}
