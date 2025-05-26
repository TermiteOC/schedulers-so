#ifndef SCHEDULE_RR_H
#define SCHEDULE_RR_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

// add a task to the list 
void add(char *name, int priority, int burst);

// invoke the scheduler
void schedule();

#endif // SCHEDULE_RR_H