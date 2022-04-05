/*
    Elysia Giannis 29694736
    Start Date: 3/10/2021 
    Date Last Modified: 8/10/2021

    Description:
    1. Opens a file provided in the command line, or if not provided, opens processes.txt in the current working directory
    2. Using the process data from the file, processes the data using the Shortest Return Time Next Techique
    3. Prints out the times processes enter the system, start running and terminate.
    4. Outputs a file results-2.txt with information regarding waiting time, turnaround time, and whether the deadlines have been met for each process.
*/

// Import all libraries and header files required
#include <stdlib.h> 
#include <stdio.h>
#include "pcb_t.h"
#include "storage.h"
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

//Creates enum for processesRunning variable
typedef enum { F, T } boolean;

//Defines the time quantum
#define timeQuantum 3

int lineCount(FILE *file){
    /*
    Counts the number of lines in given file
    :param *file: pointer to a file
    :return: integer 'count' (number of lines in file)
    */
    char c;
    int count = 1;
    for (c = getc(file); c != EOF; c = getc(file)){
        if (c == '\n') {
            count = count + 1;
        }
    }

    return count;
}

void srtn(storage **p, pcb_t **k, int count){
    /*
    Performs shortest return time next process scheduling
    :param **p: reference to array of all structs
    :param **k: reference to array of structs that have entered the system
    :param count: number of lines in file
    */

    //Declare the max number of seconds the processes can be trying to run until
    int big =1000000;
    int shortestRemainingTime;

    //Iterate through time
    for (int j=0; j<big; j++){
        //Declare the shortest remaining time as large variable
        shortestRemainingTime = 1000;
        
        //Check if any processes are currently running
        bool processesRunning;
        processesRunning = F;
        for (int m=0; m<count;m++){
            if(k[m]->state == RUNNING){
                processesRunning = T;
            }
        }

        //Add processes to ready state when they have entered
        for (int i=0; i<count; i++){
            if (p[i]->entryTime == j){
                strcpy(k[i]-> process_name, p[i]->process_name);
                k[i]-> entryTime = p[i]->entryTime;
                k[i]-> serviceTime = p[i]->serviceTime;
                k[i]->remainingTime = p[i]->serviceTime;
                k[i]-> deadline = p[i]->deadline;
                k[i]-> state = READY;
                k[i]-> waitTime = 1000000;
                printf("Time %d: %s has entered the system \n", j, k[i]->process_name);
            }
        }
        
        //Calculate the shortest remaining time out of all ready processes this second
        for (int m=0; m<count;m++){
            if((processesRunning == F) && (k[m]->state == READY) && (shortestRemainingTime > k[m]->remainingTime)){
                shortestRemainingTime = k[m]->remainingTime;
            }
        }
    
        
        for (int i=0; i<count; i++) {          
            // If its in ready state and have the shortest remaining time and no other processes are running, run it. 
            if((k[i]->state == READY) && (k[i]->remainingTime == shortestRemainingTime) && (processesRunning==F)){
                    k[i]->state = RUNNING;
                    k[i]->timeQuantumLeft = timeQuantum;
                    printf("Time %d: %s is in the running state \n", j, k[i]->process_name);
                    processesRunning = T;
                    if (k[i]->waitTime > (j-k[i]->entryTime)){
                        k[i]->waitTime = j-k[i]->entryTime;
                    }
            }

            //Decrement the timeQuantumLeft and remainingtime for the running process
            if(k[i]->state == RUNNING){
                k[i]->timeQuantumLeft = k[i]->timeQuantumLeft -1;
                k[i]->remainingTime = k[i]->remainingTime -1;
            }

            //If its running and has no more service time left, terminate it
            if((k[i]->state == RUNNING) && (k[i]->remainingTime == 0)){
                printf("Time %d: %s has finished execution \n", j+1, k[i]->process_name);
                k[i]->state = TERMINATED;
                k[i]->turnaroundTime = j-(k[i]->entryTime) + 1;
                //Check for deadline
                if (k[i]->turnaroundTime <= k[i]->deadline){
                    k[i]->deadlineMet = 1;
                }
                else{
                     k[i]->deadlineMet = 0;
                }
            }

            //If its running and it has no timeQuantum left, return it to READY
            if((k[i]->timeQuantumLeft == 0) && (k[i]->state == RUNNING)){
                k[i]->state = READY;
            }
        }
    }

}

int main(int argc, char *argv[]){
    /*
    Reads files, calls on functions to complete srtn scheduling and prints data to output file
    :param argc: number of command line arguments (including the program name)
    :param *argv[]: array of strings (character pointers) representing the individual arguments provided on the command line
    :return: 0
    */

    // Define pointer to file
    FILE *reads;

    //File not provided
    if (argc == 1){ 
        if ((reads = fopen("processes.txt", "r")) == NULL) {
            printf("Error: processes.txt file doesn't exist in the current directory or could not be opened\n");
            exit(EXIT_FAILURE);
        }
    }

    //File is provided
    else if(argc==2) { 
        if((reads = fopen(argv[1], "r")) == NULL){
            printf("Error: Provided file doesn't exist in the current directory or could not be opened\n");
            exit(EXIT_FAILURE);
        }
    }

    //Too many args provided
    else{
        printf("Error: Too many arguments provided\n");
        exit(EXIT_FAILURE);
    }

    //Identify number of processes in file
    int count;
    count = lineCount(reads);

    //Create an array of processes (structs) by allocating 'count' number of pointers
    storage **p = (storage **) malloc(count * sizeof(storage *));
    
    //Allocate count structs and have an array point to them
    for (int i=0; i<count; i++){
        p[i] = malloc(sizeof(storage));
    }

    //Return to the beginning of the file
    rewind(reads);

    // Fill structs with data
    for (int i=0; i<count; i++){
        fscanf(reads,"%s %d %d %d", p[i]->process_name, &p[i]->entryTime, &p[i]->serviceTime, &p[i]->deadline);
    }
    //Close the file
    fclose(reads);

    //Allocate count structs and have an array point to them
    pcb_t **k = (pcb_t **) malloc(count * sizeof(pcb_t *));

    //Allocate count structs and have array point to them
    for (int l=0; l<count; l++){
        k[l] = malloc(sizeof(pcb_t));
    }

    //Conducts srtn
    srtn(p, k, count);

    //Output file
    FILE *resultsFile;
    resultsFile = fopen("results-2.txt","w");
    for (int i=0; i<count; i++){
        fprintf(resultsFile,"%s %d %d %d\n", k[i]->process_name, k[i]->waitTime, k[i]->turnaroundTime, k[i]->deadlineMet);
    }
    
    return 0;
}