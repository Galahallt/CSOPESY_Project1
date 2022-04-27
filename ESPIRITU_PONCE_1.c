/**
 * @file ESPIRITU_PONCE_1.c
 * @author Paolo Espiritu (paolo_edni_v_espiritu@dlsu.edu.ph)
 * @author Andre Ponce (andre_ponce@dlsu.edu.ph)
 * @brief Implementation of CPU scheduling algorithms
 * @version 0.1
 * @date 2022-04-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ExecTime {
  int start;
  int end;
} ExecTime;

typedef struct Process {
  int id;
  int arrivalTime;
  int burstTime;
  int waitingTime;
  int execTimesLength;
  ExecTime* execTimes;
} Process;

Process* FCFS(Process* processes, int size) {
  Process* result = malloc(sizeof(Process) * size);
  int ctr = 0;
  int totalTime = 0;

  while (ctr < size) {
    // store index of prioritized process
    int index = 0;
    // initialize struct process
    Process p = { .id = 0, .arrivalTime = INT_MAX, .burstTime = 0, 
                .waitingTime = 0, .execTimesLength = 0, .execTimes = NULL };
    // get process with shortest arrival time
    for (int i = 0; i < size; i++) {
      if (processes[i].arrivalTime < p.arrivalTime) {
        p = processes[i];
        index = i;
      }
    }
    
    // execute the chosen process
    p.execTimes = realloc(NULL, sizeof(ExecTime));
    (*p.execTimes).start = totalTime;
    (*p.execTimes).end = p.burstTime + totalTime;
    p.execTimesLength++;

    p.waitingTime = totalTime - p.arrivalTime;
    totalTime += p.burstTime;
    p.burstTime = 0;
    
    p.arrivalTime = INT_MAX;

    processes[index] = p;

    result[ctr] = p;
    ctr++;
  }

  return result;
}

int main() {
  char fName[128];
  FILE *file;

  // take user input for file name
  printf("Input filename (w/o .txt): ");
  scanf("%s", fName);

  file = fopen(strcat(fName, ".txt"), "r");

  if (!file) {
    printf("%s is not found!", fName);
    exit(1);
  }
  else {
    // X denotes the CPU scheduling algorithm to be used
    // where 0 - FCFS; 1 - SJF; 2 - SRTF; 3 - RR
    // Y denotes the number of processes
    // Z denotes a time slice value (only for RR)
    int X, Y, Z;
    if (!(fscanf(file, " %d %d %d", &X, &Y, &Z))) {
      printf("Invalid inputs, check contents of %s", file);
      exit(1);
    } else {
      // retrieve Y processes in text file
      Process* p = malloc(sizeof(Process) * Y);
      int processID, arrivalTime, burstTime;
      for (int i = 0; i < Y; i++) {
        if (!(fscanf(file, " %d %d %d", &processID, &arrivalTime, &burstTime))) {
          printf("Invalid inputs, check contents of %s", file);
          exit(1);
        } else {
          p[i].id = processID;
          p[i].arrivalTime = arrivalTime;
          p[i].burstTime = burstTime;
          p[i].waitingTime = 0;
          p[i].execTimesLength = 0;
          p[i].execTimes = NULL;
        }
      }

      fclose(file);

      Process* resultFCFS = FCFS(p, Y);

      for (int j = 0; j < Y; j++) {
        printf("P[%d] ", resultFCFS[j].id);
        for (int k = 0; k < resultFCFS[j].execTimesLength; k++) {
          printf("Start Time: %d End Time: %d | ", (*(resultFCFS[j].execTimes + k)).start, (*(resultFCFS[j].execTimes + k)).end);
        }
        printf("Waiting time: %d\n", resultFCFS[j].waitingTime);
      }

      double avgTime = 0.0;
      for (int l = 0; l < Y; l++) {
        avgTime += (double) resultFCFS[l].waitingTime;
      }
      avgTime /= (double) Y;
      printf("Average waiting time: %.2f", avgTime);
    }
  }
}