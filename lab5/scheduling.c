/*******************************************************************************
*
* CprE 308 Scheduling Lab
*
* scheduling.c
*******************************************************************************/

#define NUM_PROCESSES 20
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct process
{
  /* Values initialized for each process */
  int arrivaltime;  /* Time process arrives and wishes to start */
  int runtime;      /* Time process requires to complete job */
  int priority;     /* Priority of the process */

  /* Values algorithm may use to track processes */
  int starttime;
  int endtime;
  int flag;
  int remainingtime;
};

/* Forward declarations of Scheduling algorithms */
void first_come_first_served(struct process *proc);
void shortest_remaining_time(struct process *proc);
void round_robin(struct process *proc);
void round_robin_priority(struct process *proc);

int main()
{
  int i;
  struct process proc[NUM_PROCESSES],      /* List of processes sorted based on arrival time */
                 proc_copy[NUM_PROCESSES]; /* Processes  backup*/

  /* Seed random number generator */
  /*srand(time(0));*/  /* Use this seed to test different scenarios */
  srand(0xC0FFEE);     /* Used for test to be printed out */

  /* Initialize process structures */
  int arrivaltimes[NUM_PROCESSES];
  for(i=0; i<NUM_PROCESSES; i++)
  {
    int arrivaltime = rand()%100;
    //Sort the processes
    int j;
    for (j=0;j<i;j++){
      if (proc[j].arrivaltime > arrivaltime){
        //shift the rest of the processes to the right one
        for (int k = i; k > j; k--){
            proc[k]=proc[k-1];
        }
        break;
      }
    }
    proc[j].arrivaltime = arrivaltime;
    proc[j].runtime = (rand()%30)+10;
    proc[j].priority = rand()%3;
    proc[j].starttime = 0;
    proc[j].endtime = 0;
    proc[j].flag = 0;
    proc[j].remainingtime = proc[j].runtime;
  }

  /* Show process values */
  printf("Process\tarrival\truntime\tpriority\n");
  for(i=0; i<NUM_PROCESSES; i++)
    printf("%d\t%d\t%d\t%d\n", i, proc[i].arrivaltime, proc[i].runtime,
           proc[i].priority);

  /* Run scheduling algorithms */
  printf("\n\nFirst come first served\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  first_come_first_served(proc_copy);

  printf("\n\nShortest remaining time\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  shortest_remaining_time(proc_copy);

  printf("\n\nRound Robin\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  round_robin(proc_copy);

  printf("\n\nRound Robin with priority\n");
  memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
  round_robin_priority(proc_copy);

  return 0;
}

void first_come_first_served(struct process *proc)
{
  int starttime = 1;
  int endtime = 1;
  int total_time = 0;
  for (int i=1;i<NUM_PROCESSES;i++){
    endtime = starttime + proc[i].runtime;
    total_time += endtime - proc[i].arrivaltime;
    printf("Process %d started at time %d\n", i, starttime);
    printf("Process %d ended at time %d\n", i, endtime);
    starttime = endtime;
  }
  printf("Average time from arrival to finish is %d seconds", (int) total_time/NUM_PROCESSES);

}

void shortest_remaining_time(struct process *proc)
{
  int starttime = 0;
  int endtime = 0;
  int total_time = 0;
  int shortest_remaining_time = 99999;
  int process_with_shortest_remaining_time = 0;
  int current_second = 1;
  int considered_processes = 0;
  int completed_count = 0;
  while (completed_count < NUM_PROCESSES){
    while (proc[considered_processes].arrivaltime <= current_second && considered_processes < NUM_PROCESSES){
      considered_processes++;
    }
    for (int i=0;i<considered_processes;i++){
      if (proc[i].remainingtime < shortest_remaining_time && proc[i].remainingtime!=0) {
          process_with_shortest_remaining_time = i;
          shortest_remaining_time = proc[i].remainingtime;
      }
    }
    if (proc[process_with_shortest_remaining_time].flag == 0){
      printf("Process %d started at time %d\n", process_with_shortest_remaining_time, current_second);
    }
    proc[process_with_shortest_remaining_time].flag = 1;
    proc[process_with_shortest_remaining_time].remainingtime--;
    current_second++;
    shortest_remaining_time = 99999;
    if (proc[process_with_shortest_remaining_time].remainingtime == 0){
      printf("Process %d ended at time %d\n", process_with_shortest_remaining_time, current_second);
      proc[process_with_shortest_remaining_time].endtime = current_second;
      completed_count++;
    }
    process_with_shortest_remaining_time = 0;
  }
  for (int i=0;i<NUM_PROCESSES;i++){
    total_time += proc[i].endtime - proc[i].arrivaltime;
  }
  printf("Average time from arrival to finish is %d seconds", (int) total_time/NUM_PROCESSES);

}

void round_robin(struct process *proc)
{
  int current_second = 1;
  int total_time = 0;
  int completed_count = 0;
  int current_process = 0;
  while (completed_count < NUM_PROCESSES){
    if (proc[current_process].arrivaltime<=current_second){
      if (proc[current_process].remainingtime == 0){
        current_process++;
        if (current_process > NUM_PROCESSES){
          current_process = 0;
        }
      }
      else{
        if (proc[current_process].flag == 0){
          printf("Process %d started at time %d\n", current_process, current_second);
        }
        proc[current_process].flag = 1;
        proc[current_process].remainingtime--;
        current_second++;
        if (proc[current_process].remainingtime == 0){
          proc[current_process].endtime = current_second;
          printf("Process %d ended at time %d\n", current_process, current_second);
          completed_count++;
        }
        current_process++;
        if (current_process > NUM_PROCESSES){
          current_process = 0;
        }
      }
    }
    else {
      current_process=0;
    }
  }
  for (int i=0;i<NUM_PROCESSES;i++){
    total_time += proc[i].endtime - proc[i].arrivaltime;
  }
  printf("Average time from arrival to finish is %d seconds", (int) total_time/NUM_PROCESSES);

}

void round_robin_priority(struct process *proc)
{
  int starttime = 0;
  int endtime = 0;
  int total_time = 0;
  int highest_priority = 0;
  int current_second = 1;
  int considered_processes = 0;
  int completed_count = 0;
  int current_process_by_priority[3] = {-1,-1,-1};
  while (completed_count < NUM_PROCESSES){
    while (proc[considered_processes].arrivaltime <= current_second && considered_processes < NUM_PROCESSES){
      considered_processes++;
    }
    for (int i=0;i<considered_processes;i++){
      if (proc[i].priority > highest_priority){
        highest_priority = proc[i].priority;
      }
    }
    while (1){
      //find the next process with highest priority
      int current_process = current_process_by_priority[highest_priority];
      if (proc[current_process].priority == highest_priority && proc[current_process].remainingtime != 0){
        if (proc[current_process].flag == 0){
          printf("Process %d started at time %d\n", current_process, current_second);
        }
        proc[current_process].flag = 1;
        proc[current_process].remainingtime--;
        current_second++;
        if (proc[current_process].remainingtime == 0){
          proc[current_process].endtime = current_second;
          printf("Process %d ended at time %d\n", current_process, current_second);
          completed_count++;
        }
        highest_priority = 0;
        break;
      }
      current_process_by_priority[highest_priority]++;
      if (current_process_by_priority[highest_priority] > NUM_PROCESSES){
        current_process_by_priority[highest_priority] = 0;
      }
    }
  }
}




