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
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0

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

// Performs the First-Come First-Served Scheduling Algorithm.
Process* performFCFS(Process* processes, int size) {
  Process* result = malloc(sizeof(Process) * size); // allocate memory for result of process
  
  int ctr = 0;  // keep track of the number of processes

  int totalTime = 0; // initialize total execution time of the processes

  while (ctr < size) {
    // store index of prioritized process
    int index = 0;
    // initialize process with a temporary arrivalTime reference
    Process p = processes[0];
    // get process with shortest arrival time
    for (int i = 0; i < size; i++) {
      if (processes[i].arrivalTime < p.arrivalTime) {
        p = processes[i];
        index = i;
      }
    }
    
    // execute the chosen process

    // allocate memory to start and end time struct and assign corresponding values
    p.execTimes = realloc(NULL, sizeof(ExecTime));
    (*p.execTimes).start = totalTime;
    (*p.execTimes).end = p.burstTime + totalTime;

    // update number of execution of the process
    p.execTimesLength++;

    // update waiting and burst time of the process and total time of execution
    p.waitingTime = totalTime - p.arrivalTime;
    totalTime += p.burstTime;
    p.burstTime = 0;
    
    p.arrivalTime = INT_MAX; // update arrival time of process to sentinel value
    
    processes[index] = p; // update process with new process values

    result[ctr] = p;  // store current process to the result

    ctr++; // increment counter for process
  }

  return result; // return the result which is an array of updated processes 
}

// Performs the Shortest Job First Scheduling Algorithm.
Process* performSJF(Process* processes, int size) {
  Process* result = malloc(sizeof(Process) * size); // allocate memory for result of process
  
  int ctr = 0;  // keep track of the number of processes

  int totalTime = 0; // initialize total execution time of the processes

  while (ctr < size) {
    // store index of prioritized process
    int index = 0;
    // initialize process with a temporary arrivalTime reference
    Process p = processes[0];
    // get process with shortest arrival time
    for (int i = 0; i < size; i++) {
      // make sure that the process is already in the waiting queue
      if (processes[i].arrivalTime <= totalTime) {
          // prioritize lower burst time
          if (processes[i].burstTime < p.burstTime) {
            p = processes[i];
            index = i;
          }
          // if equal burst time, prioritize arrival time
          else if (processes[i].burstTime == p.burstTime && processes[i].arrivalTime < p.arrivalTime) {
            p = processes[i];
            index = i;
          }
        }
      }
    // execute the chosen process

    // allocate memory to start and end time struct and assign corresponding values
    p.execTimes = realloc(NULL, sizeof(ExecTime));
    (*p.execTimes).start = totalTime;
    (*p.execTimes).end = p.burstTime + totalTime;

    // update number of execution of the process
    p.execTimesLength++;

    // update waiting and burst time of the process and total time of execution
    p.waitingTime = totalTime - p.arrivalTime;
    totalTime += p.burstTime;
    p.burstTime = INT_MAX;
    
    p.arrivalTime = 0; // update arrival time of process to sentinel value
    
    processes[index] = p; // update process with new process values

    result[ctr] = p;  // store current process to the result

    ctr++; // increment counter for process
  }

  return result; // return the result which is an array of updated processes 
}

// Performs the Round Robin Scheduling Algorithm.
Process* performRR (Process* processes, int processAmt, int quantum) {
  // array containing the results
  Process* result = malloc(sizeof(Process) * processAmt);
  // counter for the processes that finished executing
  int processCtr = 0;
  // current time of execution
  int currTime = 0;
  // index of process being executed
  int index = 0;

  while (processCtr < processAmt) {
    // get current process
    Process p = processes[index];

    // if process has arrived and is still not finished executing
    if (p.arrivalTime <= currTime && p.burstTime > 0) {
      // allocate for start-end times
      p.execTimes = realloc(p.execTimes, sizeof(ExecTime) * (p.execTimesLength + 1));

      // compute for waiting time value to be added
      int waitingTimeVal = (p.burstTime >= quantum ? quantum : p.burstTime);

      // set start time
      p.execTimes[p.execTimesLength].start = currTime;
      // set end time
      p.execTimes[p.execTimesLength].end = currTime + waitingTimeVal;

      // increase wait times of all other processes
      for (int i = 0; i < processAmt; i++) {
        /**
          * If process is:
          * - not the same as the executing process
          * - not yet finished executing
          * - has arrived already
          * then increase its waiting time
          */
        if (processes[i].id != p.id && 
            processes[i].burstTime > 0 && 
            processes[i].arrivalTime <= currTime) {
          processes[i].waitingTime += waitingTimeVal;
        }
      }

      // decrease remaining time of process
      p.burstTime -= waitingTimeVal;
      // +1 in no. of times executed for this process
      p.execTimesLength++;

      // update process in input list
      processes[index] = p;

      // increase time by waiting time value
      currTime += waitingTimeVal;
      
      // if process is finished executing
      if (p.burstTime <= 0) {
        // store process in result index
        result[index] = p;
        // increment no. of processes finished
        processCtr++;
      }
    }

    // if index exceeds bounds, reset to 0
    if (index >= processAmt) {
      int hasExecutableProcess = FALSE;
      // traverse processes array and check for any executable processes
      for (int i = 0; i < processAmt; i++) 
        if (processes[i].arrivalTime <= currTime && processes[i].burstTime > 0) {
          hasExecutableProcess = TRUE;
          break;
        }

      // if there are no executable processes, advance time
      if (!hasExecutableProcess) 
        currTime++;

      // reset index
      index = 0;
    }
    // else, increment
    else
      index++;
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
      printf("Invalid inputs, check contents of %s", fName);
      exit(1);
    } else {
      // retrieve Y processes in text file
      Process* p = malloc(sizeof(Process) * Y);
      int processID, arrivalTime, burstTime;
      for (int i = 0; i < Y; i++) {
        // validate inputs of text file
        if (!(fscanf(file, " %d %d %d", &processID, &arrivalTime, &burstTime))) {
          printf("Invalid inputs, check contents of %s", fName);
          exit(1);
        } else {
          // store inputs from text file to process struct
          p[i].id = processID;
          p[i].arrivalTime = arrivalTime;
          p[i].burstTime = burstTime;
          p[i].waitingTime = 0;
          p[i].execTimesLength = 0;
          p[i].execTimes = NULL;
        }
      }

      // close file
      fclose(file);

      // store results here
      Process* results;
      // switch-case for various scheduling algorithms
      switch (X) {
        case 0:
          results = performFCFS(p, Y);
          break;
        case 1:
          results = performSJF(p, Y);
          break;
        case 2:
          // results = performSRTF(p, Y);
          break;
        case 3:
          results = performRR(p, Y, Z);
          break;
      }

      // print start time/s, end time/s, and waiting time
      for (int j = 0; j < Y; j++) {
        printf("P[%d] ", results[j].id);
        for (int k = 0; k < results[j].execTimesLength; k++) {
          printf("Start Time: %d End Time: %d | ", (*(results[j].execTimes + k)).start, (*(results[j].execTimes + k)).end);
        }
        printf("Waiting time: %d\n", results[j].waitingTime);
      }

      // compute the average waiting time of the scheduling algorithm used
      double avgTime = 0.0;
      for (int l = 0; l < Y; l++) {
        avgTime += (double) results[l].waitingTime;
      }
      avgTime /= (double) Y;
      printf("Average waiting time: %.2f", avgTime);
    }
  }
}