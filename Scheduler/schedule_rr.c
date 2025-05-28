#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "CPU.h"
#include "schedule_rr.h"

struct node *head = NULL;

void add(char *name, int priority, int burst) {
    Task *t = malloc(sizeof(Task));
    t->name = name;
    t->priority = priority;
    t->burst = burst;

    insert(&head, t);
}

void schedule() {
    while (head != NULL) {
        struct node *temp = head;

        while (temp != NULL) {
            Task *t = temp->task;
            int slice = (t->burst > QUANTUM) ? QUANTUM : t->burst;
            run(t, slice);
            t->burst -= slice;

            struct node *next = temp->next;

            if (t->burst <= 0) {
                delete(&head, t);
                free(t);
            }

            temp = next;
        }
    }
}
