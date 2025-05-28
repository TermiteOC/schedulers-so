#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "CPU.h"
#include "schedule_pa.h"
#include "timer.h"

#define AGING_LIMIT 3  // Após 3 ciclos sem executar, a prioridade sobe

struct node *fila[MAX_PRIORITY + 1] = {NULL};

void add(char *name, int priority, int burst) {
    if (priority < MIN_PRIORITY) priority = MIN_PRIORITY;
    if (priority > MAX_PRIORITY) priority = MAX_PRIORITY;

    Task *t = malloc(sizeof(Task));
    t->name = strdup(name);
    t->priority = priority;
    t->burst = burst;
    t->waitingTime = 0;

    struct node *newNode = malloc(sizeof(struct node));
    newNode->task = t;
    newNode->next = NULL;

    if (fila[priority] == NULL) {
        fila[priority] = newNode;
    } else {
        struct node *temp = fila[priority];
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

void apply_aging(Task *executed_task) {
    for (int p = MIN_PRIORITY + 1; p <= MAX_PRIORITY; p++) {
        struct node *prev = NULL;
        struct node *curr = fila[p];

        while (curr != NULL) {
            if (curr->task == executed_task) {
                prev = curr;
                curr = curr->next;
                continue;
            }

            curr->task->waitingTime++;

            if (curr->task->waitingTime >= AGING_LIMIT) {
                Task *t = curr->task;
                if (t->priority > MIN_PRIORITY) {
                    t->priority--;
                }
                t->waitingTime = 0;

                if (prev == NULL) {
                    fila[p] = curr->next;
                } else {
                    prev->next = curr->next;
                }

                struct node *temp = curr;
                curr = curr->next;
                free(temp);

                struct node *newNode = malloc(sizeof(struct node));
                newNode->task = t;
                newNode->next = NULL;

                if (fila[p - 1] == NULL) {
                    fila[p - 1] = newNode;
                } else {
                    struct node *t2 = fila[p - 1];
                    while (t2->next != NULL) {
                        t2 = t2->next;
                    }
                    t2->next = newNode;
                }
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    }
}

struct node *find_shortest_task_node(struct node *head, struct node **prev_out) {
    struct node *min_node = head;
    struct node *min_prev = NULL;

    struct node *curr = head;
    struct node *prev = NULL;

    while (curr != NULL) {
        if (curr->task->burst < min_node->task->burst) {
            min_node = curr;
            min_prev = prev;
        }
        prev = curr;
        curr = curr->next;
    }

    if (prev_out != NULL) {
        *prev_out = min_prev;
    }

    return min_node;
}

void schedule() {
    start_timer();

    int tasks_remaining = 1;

    while (tasks_remaining) {
        tasks_remaining = 0;

        for (int p = MIN_PRIORITY; p <= MAX_PRIORITY; p++) {
            if (fila[p] != NULL) {
                tasks_remaining = 1;

                struct node *prev = NULL;
                struct node *shortest = find_shortest_task_node(fila[p], &prev);

                Task *t = shortest->task;
                int slice = (t->burst > QUANTUM) ? QUANTUM : t->burst;

                run(t, slice);
                t->burst -= slice;

                t->waitingTime = 0;

                if (prev == NULL) {
                    fila[p] = shortest->next;
                } else {
                    prev->next = shortest->next;
                }

                free(shortest);

                if (t->burst > 0) {
                    struct node *newNode = malloc(sizeof(struct node));
                    newNode->task = t;
                    newNode->next = NULL;

                    if (fila[p] == NULL) {
                        fila[p] = newNode;
                    } else {
                        struct node *temp = fila[p];
                        while (temp->next != NULL) {
                            temp = temp->next;
                        }
                        temp->next = newNode;
                    }
                } else {
                    free(t);
                }

                apply_aging(t);

                break;
            }
        }
    }

    stop_timer();
}