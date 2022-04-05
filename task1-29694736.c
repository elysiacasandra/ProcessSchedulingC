/*
    Elysia Giannis 29694736
    Start Date: 3/10/2021 
    Date Last Modified: 8/10/2021

    Description:
    1. Opens a file provided in the command line, or if not provided, opens processes.txt in the current working directory
    2. Using the process data from the file, processes the data using the First Come First Serve Techique
    3. Prints out the times processes enter the system, start running and terminate.
    4. Outputs a file results-1.txt with information regarding waiting time, turnaround time, and whether the deadlines have been met for each process.
*/

// Import all libraries and header files required
#include <stdlib.h> 
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include "pcb_t.h"
#include "storage.h"

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

void fcfs(storage **p, pcb_t **k, int count){
    /*
    Performs first come first serve process scheduling
    :param **p: reference to array of all structs
    :param **k: reference to array of structs that have entered the system
    :param count: number of lines in file
    */

   //Declare the max number of seconds the processes can be trying to run until
    int big =1000000;

    //Iterate through time
    for (int j=0; j<big; j++){

        //Iterate through the processes
        for (int i=0; i<count; i++){
            //Add values to pcb_t struct if its entered the system 
            if (p[i]->entryTime == j){
                strcpy(k[i]-> process_name, p[i]->process_name);
                k[i]-> entryTime = p[i]->entryTime;
                k[i]-> serviceTime = p[i]->serviceTime;
                k[i]-> deadline = p[i]->deadline;
                k[i]-> state = READY;
                printf("Time %d: %s has entered the system \n", j, k[i]->process_name);
            }

            //If its the first process and in ready state, run it
            if((i==0) && (k[i]->entryTime == j)){
                k[i]->state = RUNNING;
                k[i]->startedRunning = j;
                k[i]->waitTime = j - (k[i]->entryTime);
                printf("Time %d: %s is in the running state \n", j, k[i]->process_name);
            }
            //If its not the first process, its in ready state and the one before has terminated, run it
            else if((i!=0) && ((k[i-1]->state == TERMINATED) &&  (k[i]->state == READY))){
                k[i]->state = RUNNING;
                k[i]->startedRunning = j;
                k[i]->waitTime = j - k[i]->entryTime;
                printf("Time %d: %s is in the running state \n", j, k[i]->process_name);
            }

            //If its in running and its run to completion, terminate
            if((k[i]->state == RUNNING) && (((k[i]->startedRunning) + (k[i]->serviceTime)) == j)){
                k[i]->terminatedTime = j;
                //Check if its met the deadline
                if ((k[i]->terminatedTime - k[i]->entryTime) <= k[i]->deadline){
                    k[i]->deadlineMet = 1;
                }
                else{
                     k[i]->deadlineMet = 0;
                }
                printf("Time %d: %s has finished execution \n", j, k[i]->process_name);
                k[i]->state = TERMINATED;
                k[i]->turnaroundTime = j-(k[i]->entryTime);

                //Exit the for loop
                if (i==count){
                    j=big;
                }
            }
            
        }
    }
}

int main(int argc, char *argv[]){
    /*
    Reads files, calls on functions to complete fcfs scheduling and prints data to output file
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

    //Conducts fcfs 
    fcfs(p, k, count);

    //Output file
    FILE *resultsFile;
    resultsFile = fopen("results-1.txt","w");
    for (int i=0; i<count; i++){
        fprintf(resultsFile,"%s %d %d %d\n", k[i]->process_name, k[i]->waitTime, k[i]->turnaroundTime, k[i]->deadlineMet);
    }

    return 0;
}

