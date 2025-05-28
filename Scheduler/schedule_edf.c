#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "timer.h"
#include "task.h"
#include "list.h"
#include "CPU.h"
#include "schedule_edf.h"

struct node *head = NULL;

void add_edf(char *name, int priority, int burst, int deadline) {
    Task *t = malloc(sizeof(Task));
    t->name = strdup(name);
    t->priority = priority;
    t->burst = burst;
    t->deadline = deadline;

    insert(&head, t);
}

struct node* find_earliest_valid_deadline() {
    struct node *curr = head;
    struct node *selected = NULL;

    int min_deadline = __INT_MAX__;
    while (curr != NULL) {
        Task *t = curr->task;
        if (t->burst > 0 && t->deadline >= time_elapsed && t->deadline < min_deadline) {
            min_deadline = t->deadline;
            selected = curr;
        }
        curr = curr->next;
    }

    return selected;
}

void remove_expired_tasks() {
    struct node *curr = head;
    struct node *prev = NULL;

    while (curr != NULL) {
        Task *t = curr->task;

        if (t->deadline < time_elapsed) {
            printf("Tarefa \"%s\" descartada (deadline %d < tempo %d)\n", t->name, t->deadline, time_elapsed);
            struct node *to_delete = curr;
            curr = curr->next;

            delete(&head, t);
            free(to_delete->task->name);
            free(to_delete->task);
            free(to_delete);
        } else {
            curr = curr->next;
        }
    }
}

void schedule() {
    start_timer();

    while (head != NULL) {
        remove_expired_tasks();

        struct node *n = find_earliest_valid_deadline();
        if (n == NULL) break;

        Task *t = n->task;
        int slice = (t->burst > QUANTUM) ? QUANTUM : t->burst;

        run(t, slice);
        t->burst -= slice;

        usleep(slice * 1000);
        time_elapsed += slice;

        if (t->burst <= 0) {
            delete(&head, t);
            free(t->name);
            free(t);
        }
    }

    stop_timer();
}

void add(char *name, int priority, int burst) {
}
