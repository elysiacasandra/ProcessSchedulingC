/* Special enumerated data type for process state */ 
typedef enum {
    NOTREADY, READY , RUNNING , TERMINATED
} process_state_t ;

/* C data structure used as process control block . The scheduler
* should create one instance per running process in the system.
*/
enum process_state_t;
typedef struct {
    char process_name [11]; // A string that identifies the process /* Times are measured in seconds. */
    int entryTime; // The time process entered system
    int serviceTime; // The total CPU time required by the process 
    int deadline;
    int timeQuantumLeft;
    int remainingTime ; // Remaining service time until completion .
    process_state_t state; //current process state (e.g. READY). 
    int startedRunning;
    int terminatedTime;
    int waitTime;
    int turnaroundTime;
    int deadlineMet;
} pcb_t;

struct pcb_t;
