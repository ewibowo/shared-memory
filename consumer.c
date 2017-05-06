/* Program: consumer.c
 * This program creates a shared memory segment and displays any data written 
 * to the shared memory segment by a producer process. 
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "comm.h"

int main()
{

    /* We create a shared memory segment with shared memory identifier SHM_ID 
    defined in comm.h. The same SHM_ID number is used by this process and the
    producer process. */

    int shmid;
    shmid = shmget((key_t) SHM_ID, sizeof(shared_use_t), 0666 | IPC_CREAT);

    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    /* We now make the shared memory segment accessible to the program 
    by attaching a pointer to the shared memory segment. */
    void *shared_memory = NULL;
    shared_memory = shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Shared memory attached at %X\n", (int)shared_memory);

    /* We craete a struct reference to the shared memory segment that will be 
    shared by this process and producer process. */
    shared_use_t *shared_data = NULL;
    shared_data = (shared_use_t *)shared_memory;
    shared_data->readable = FALSE;

    /* Consumer loop reads text messages entered in the producer process. */
    int running = 1;
    while(running) {
        if (shared_data->readable) {
            printf("Get: %s", shared_data->shared_text);

            /* Set the readable flag to FALSE to indicate that the data have been 
            read and the shared_text variable is ready to be over-written by 
            the producer process */
            shared_data->readable = FALSE;
            if (strncmp(shared_data->shared_text, "end", 3) == 0) {
                running = 0;
            }
        }
    }

    /* The shared memory is detached. */
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    /* Lastly, the shared memory segment is deleted.  */
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
