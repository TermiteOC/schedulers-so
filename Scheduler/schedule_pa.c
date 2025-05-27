#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "CPU.h"
#include "schedule_pa.h"
#include "timer.h"

#define AGING_LIMIT 10  // Após 7 ciclos sem executar, a prioridade sobe

// Múltiplas filas, uma para cada nível de prioridade
struct node *fila[MAX_PRIORITY + 1] = {NULL};

// Adiciona uma nova task à fila correspondente
void add(char *name, int priority, int burst) {
    if (priority < MIN_PRIORITY) priority = MIN_PRIORITY;
    if (priority > MAX_PRIORITY) priority = MAX_PRIORITY;

    Task *t = malloc(sizeof(Task));
    t->name = strdup(name);
    t->priority = priority;
    t->burst = burst;
    t->waitingTime = 0; // Usado para aging

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

// Aging: aumenta a prioridade das tasks que estão esperando (ignora a executada)
void apply_aging(Task *executed_task) {
    for (int p = MIN_PRIORITY + 1; p <= MAX_PRIORITY; p++) {
        struct node *prev = NULL;
        struct node *curr = fila[p];

        while (curr != NULL) {
            if (curr->task == executed_task) {
                // Ignora aging para a task que acabou de executar
                prev = curr;
                curr = curr->next;
                continue;
            }

            curr->task->waitingTime++;

            if (curr->task->waitingTime >= AGING_LIMIT) {
                // Sobe prioridade, mas não pode ultrapassar MIN_PRIORITY
                Task *t = curr->task;
                if (t->priority > MIN_PRIORITY) {
                    t->priority--;
                }
                t->waitingTime = 0;

                // Remove da fila atual
                if (prev == NULL) {
                    fila[p] = curr->next;
                } else {
                    prev->next = curr->next;
                }

                struct node *temp = curr;
                curr = curr->next;

                free(temp);

                // Insere na fila de prioridade superior (no fim)
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

// Função principal de escalonamento FCFS com prioridade e aging
void schedule() {
    start_timer();

    int tasks_remaining = 1;

    while (tasks_remaining) {
        tasks_remaining = 0;

        // Percorre as filas da maior prioridade para a menor
        for (int p = MIN_PRIORITY; p <= MAX_PRIORITY; p++) {
            if (fila[p] != NULL) {
                tasks_remaining = 1;

                Task *t = fila[p]->task;
                int slice = t->burst;  // FCFS: executa task até o fim do burst

                run(t, slice);
                t->burst -= slice;

                // Resetar waitingTime da task executada para 0
                t->waitingTime = 0;

                // Remove a task da fila
                struct node *oldHead = fila[p];
                fila[p] = fila[p]->next;
                free(oldHead);

                if (t->burst > 0) {
                    // FCFS executa até acabar a task, então aqui ela terminou
                    // (não deve acontecer, pois slice = burst)
                } else {
                    free(t); // Task finalizada
                }

                // Aplica aging após executar uma task, ignorando a executada
                apply_aging(t);

                // Sai do for para sempre priorizar a maior prioridade disponível
                break;
            }
        }
    }

    stop_timer();
}
