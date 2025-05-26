#ifndef SCHEDULE_RR_P_H
#define SCHEDULE_RR_P_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10
#define QUANTUM 10

void add(char *name, int priority, int burst);
void schedule();

#endif // SCHEDULE_RR_P_H
