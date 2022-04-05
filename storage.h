typedef struct {
    char process_name [11]; // A string that identifies the process /* Times are measured in seconds. */
    int entryTime; // The time process entered system
    int serviceTime;
    int deadline;
} storage;

struct storage;