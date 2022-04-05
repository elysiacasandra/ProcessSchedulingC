/*
    Elysia Giannis 29694736
    Start Date: 3/10/2021 
    Date Last Modified: 8/10/2021

    Description:
    1. Opens a file provided in the command line, or if not provided, opens processes.txt in the current working directory
    2. Using the process data from the file, processes the data using own deadline based testing technique
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

void dbs(storage **p, pcb_t **k, int count){
    /*
    Performs deadline based process scheduling
    :param **p: reference to array of all structs
    :param **k: reference to array of structs that have entered the system
    :param count: number of lines in file
    */

    //Declare the max number of seconds the processes can be trying to run until
    int big =1000000;
    int shortestRemainingDeadline;
    
    //Iterate through time
    for (int j=0; j<big; j++){

        //Declare the shortest remaining time as large variable
        shortestRemainingDeadline = 1000;

        //Initiate processesRunning to F
        bool processesRunning;
        processesRunning = F;

        //Add processes to ready state when they have entered
        for (int i=0; i<count; i++){
            if (p[i]->entryTime == j){
                strcpy(k[i]-> process_name, p[i]->process_name);
                k[i]-> entryTime = p[i]->entryTime;
                k[i]-> serviceTime = p[i]->serviceTime;
                k[i]->remainingTime = p[i]->serviceTime;
                k[i]-> deadline = p[i]->deadline;
                k[i]->waitTime = 1000000;
                k[i]-> state = READY;
                printf("Time %d: %s has entered the system \n", j, k[i]->process_name);
            }
        }
        //Check how many processes are in ready state at this time.
        int noReadyState=0;
        for (int i=0; i<count; i++){
            if ((k[i]->entryTime <= j) && (k[i]->state != TERMINATED)){
                noReadyState = noReadyState+1;
            }
        }

        //Check how many processes have the ability to meet their deadline at this time.
        int noCanMeetDeadline = 0;
        for (int i=0; i<count; i++){
            if (((k[i]->deadline >= k[i]->remainingTime)) && (k[i]->state == READY)){
                noCanMeetDeadline = noCanMeetDeadline+1;
            }
        }
        
        // Calculate the shortest remaining deadline out of all the processes that can meet their deadline
        for (int m=0; m<count;m++){
            if((k[m]->deadline >= k[m]->remainingTime) && (k[m]->state == READY) && (shortestRemainingDeadline > k[m]->deadline)){
                shortestRemainingDeadline = k[m]->deadline;
            }
        }
    
        for (int i=0; i<count; i++) {  
            //If the process is in ready state and has the shortest remaining deadline run it
            // OR if the process is the only process in ready state, run it
            // OR if its in ready state and no ready processes can meet the deadline, run it.         
            if(((k[i]->state == READY) && (k[i]->deadline == shortestRemainingDeadline) && (processesRunning == F) && (k[i]->deadline >= k[i]->remainingTime)) || ((k[i]->state == READY) && (noReadyState==1)) || ((k[i]->state == READY) && (noCanMeetDeadline == 0))){
                k[i]->state = RUNNING;
                printf("Time %d: %s is in the running state \n", j, k[i]->process_name);
                processesRunning = T;
                if (k[i]->waitTime > (j-k[i]->entryTime)){
                    k[i]->waitTime = j-k[i]->entryTime;
                }
            }

            //Decrement the remaining time
            if(k[i]->state == RUNNING){
                k[i]->remainingTime = k[i]->remainingTime -1;

            }

            //If its in running state and has finished execution, termination.
            if((k[i]->state == RUNNING) && (k[i]->remainingTime == 0)){
                printf("Time %d: %s has finished execution \n", j+1, k[i]->process_name);
                k[i]->state = TERMINATED;
                k[i]->turnaroundTime = j-(k[i]->entryTime) + 1;
                //Check if deadline has been met
                if (k[i]->turnaroundTime <= p[i]->deadline){
                    k[i]->deadlineMet = 1;
                }
                else{
                     k[i]->deadlineMet = 0;
                }
            }

            //Automatically convert it back to ready state as the time quantum is 1.
            if(k[i]->state == RUNNING){
                k[i]->state = READY;
            }

            //Decrement the deadline time.
            if(k[i]-> state == READY){
                k[i]->deadline = k[i]->deadline -1;
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

    //Conducts deadline based sorting
    dbs(p, k, count);

    //Output file
    FILE *resultsFile;
    resultsFile = fopen("results-3.txt","w");
    for (int i=0; i<count; i++){
        fprintf(resultsFile,"%s %d %d %d\n", k[i]->process_name, k[i]->waitTime, k[i]->turnaroundTime, k[i]->deadlineMet);
    }
    
    return 0;
}