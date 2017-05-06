/* Program: producer.c 
 * This program attaches to an existing shared memory segment and allows the user 
 * to enter data to the shared memory segment. 
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
    // A buffer to contain the shared text
    char buffer[BUFSIZ];

    /* We create a shared memory segment with shared memory identifier SHM_ID 
    defined in comm.h. The same SHM_ID number is used by this process and the
    customer process. */
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
    
    /* We create a struct reference to the shared memory segment that will be 
    shared by this process and the consumer process. */
    shared_use_t *shared_data;
    shared_data = (shared_use_t *)shared_memory;

    /* Producer loop reads text messages entered by the user. */
    int running = 1;
    while(running) {
        while(shared_data->readable == TRUE) {
            sleep(1);            
            printf("waiting for consumer...\n");
        }
        printf("Enter some text: ");
        fgets(buffer, BUFSIZ, stdin);
        
        strncpy(shared_data->shared_text, buffer, TEXT_SZ);

        /* Set readable flag to TRUE to tell consumer process that the data is 
        ready to be read. */
        shared_data->readable = TRUE;

        if (strncmp(buffer, "end", 3) == 0) {
                running = 0;
        }
    }

    /* The shared memory is detached and will be deleted by the consumer process. */
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
