/* A common header file to describe the shared data. */

#define TEXT_SZ 2048
#define SHM_ID 1234

typedef enum boolean { FALSE, TRUE } bool_t;

typedef struct shared_use {
    // flag to indicate that the data is ready to be read
    bool_t readable;

    // shared data
    char shared_text[TEXT_SZ];
} shared_use_t;

